/*  This file is part of the KDE project
    Copyright (C) 2006 Tim Beaulen <tbscope@gmail.com>
    Copyright (C) 2006-2007 Matthias Kretz <kretz@kde.org>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.

*/

#include "effect.h"
#include <kdebug.h>
#include <klocale.h>
#include <QVariant>
#include "xineengine.h"
#include <QMutexLocker>
#include "events.h"
#include "keepreference.h"

namespace Phonon
{
namespace Xine
{

xine_audio_port_t *EffectXT::audioPort() const
{
    const_cast<EffectXT *>(this)->ensureInstance();
    Q_ASSERT(m_plugin);
    Q_ASSERT(m_plugin->audio_input);
    Q_ASSERT(m_plugin->audio_input[0]);
    return m_plugin->audio_input[0];
}

xine_post_out_t *EffectXT::audioOutputPort() const
{
    const_cast<EffectXT *>(this)->ensureInstance();
    Q_ASSERT(m_plugin);
    xine_post_out_t *x = xine_post_output(m_plugin, "audio out");
    Q_ASSERT(x);
    return x;
}

void EffectXT::rewireTo(SourceNodeXT *source)
{
    if (!source->audioOutputPort()) {
        return;
    }
    ensureInstance();
    xine_post_in_t *x = xine_post_input(m_plugin, "audio in");
    Q_ASSERT(x);
    xine_post_wire(source->audioOutputPort(), x);
}

// lazy initialization
void EffectXT::ensureInstance()
{
    QMutexLocker lock(&m_mutex);
    if (m_plugin) {
        return;
    }
    createInstance();
    Q_ASSERT(m_plugin);
}

xine_audio_port_t *EffectXT::fakeAudioPort()
{
    if (!m_fakeAudioPort) {
        m_fakeAudioPort = xine_open_audio_driver(m_xine, "none", 0);
    }
    return m_fakeAudioPort;
}

void EffectXT::createInstance()
{
    kDebug(610) << "m_pluginName =" << m_pluginName;
    Q_ASSERT(m_plugin == 0 && m_pluginApi == 0);
    if (!m_pluginName) {
        kWarning(610) << "tried to create invalid Effect";
        return;
    }

    fakeAudioPort();
    m_plugin = xine_post_init(m_xine, m_pluginName, 1, &m_fakeAudioPort, 0);
    xine_post_in_t *paraInput = xine_post_input(m_plugin, "parameters");
    if (!paraInput) {
        return;
    }
    Q_ASSERT(paraInput->type == XINE_POST_DATA_PARAMETERS);
    Q_ASSERT(paraInput->data);
    m_pluginApi = reinterpret_cast<xine_post_api_t *>(paraInput->data);
    if (!m_parameterList.isEmpty()) {
        return;
    }
    xine_post_api_descr_t *desc = m_pluginApi->get_param_descr();
    if (m_pluginParams) {
        m_pluginApi->set_parameters(m_plugin, m_pluginParams);
    } else {
        m_pluginParams = static_cast<char *>(malloc(desc->struct_size));
        m_pluginApi->get_parameters(m_plugin, m_pluginParams);
        for (int i = 0; desc->parameter[i].type != POST_PARAM_TYPE_LAST; ++i) {
            xine_post_api_parameter_t &p = desc->parameter[i];
            switch (p.type) {
            case POST_PARAM_TYPE_INT:          /* integer (or vector of integers)    */
                if (p.enum_values) {
                    // it's an enum
                    QVariantList values;
                    for (int j = 0; p.enum_values[j]; ++j) {
                        values << QString::fromUtf8(p.enum_values[j]);
                    }
                    m_parameterList << EffectParameter(i, QString::fromUtf8(p.description ? p.description : p.name), 0,
                            *reinterpret_cast<int *>(m_pluginParams + p.offset),
                            0, values.count() - 1, values);
                } else {
                    m_parameterList << EffectParameter(i, QString::fromUtf8(p.description ? p.description : p.name), EffectParameter::IntegerHint,
                            *reinterpret_cast<int *>(m_pluginParams + p.offset),
                            static_cast<int>(p.range_min), static_cast<int>(p.range_max), QVariantList());
                }
                break;
            case POST_PARAM_TYPE_DOUBLE:       /* double (or vector of doubles)      */
                m_parameterList << EffectParameter(i, QString::fromUtf8(p.description ? p.description : p.name), 0,
                        *reinterpret_cast<double *>(m_pluginParams + p.offset),
                        p.range_min, p.range_max, QVariantList());
                break;
            case POST_PARAM_TYPE_CHAR:         /* char (or vector of chars = string) */
            case POST_PARAM_TYPE_STRING:       /* (char *), ASCIIZ                   */
            case POST_PARAM_TYPE_STRINGLIST:   /* (char **) list, NULL terminated    */
                kWarning(610) << "char/string/stringlist parameter '" << (p.description ? p.description : p.name) << "' not supported.";
                break;
            case POST_PARAM_TYPE_BOOL:         /* integer (0 or 1)                   */
                m_parameterList << EffectParameter(i, QString::fromUtf8(p.description ? p.description : p.name), EffectParameter::ToggledHint,
                        static_cast<bool>(*reinterpret_cast<int *>(m_pluginParams + p.offset)),
                        QVariant(), QVariant(), QVariantList());
                break;
            case POST_PARAM_TYPE_LAST:         /* terminator of parameter list       */
            default:
                abort();
            }
        }
    }
}

Effect::Effect(int effectId, QObject *parent)
    : QObject(parent),
    SinkNode(new EffectXT(0)),
    SourceNode(static_cast<EffectXT *>(SinkNode::threadSafeObject().data()))
{
    K_XT(Effect);
    const char *const *postPlugins = xine_list_post_plugins_typed(xt->m_xine, XINE_POST_TYPE_AUDIO_FILTER);
    if (effectId >= 0x7F000000) {
        effectId -= 0x7F000000;
        for(int i = 0; postPlugins[i]; ++i) {
            if (i == effectId) {
                // found it
                xt->m_pluginName = postPlugins[i];
                break;
            }
        }
    }
}

Effect::Effect(EffectXT *xt, QObject *parent)
    : QObject(parent), SinkNode(xt), SourceNode(xt)
{
}

EffectXT::EffectXT(const char *name)
    : SourceNodeXT("Effect"), SinkNodeXT("Effect"), m_plugin(0), m_pluginApi(0), m_fakeAudioPort(0),
    m_pluginName(name), m_pluginParams(0)
{
    m_xine = Backend::xine();
}

EffectXT::~EffectXT()
{
    if (m_plugin) {
        xine_post_dispose(m_xine, m_plugin);
        m_plugin = 0;
        m_pluginApi = 0;
        if (m_fakeAudioPort) {
            xine_close_audio_driver(m_xine, m_fakeAudioPort);
            m_fakeAudioPort = 0;
        }
    }
    free(m_pluginParams);
    m_pluginParams = 0;
}

bool Effect::isValid() const
{
    K_XT(const Effect);
    return xt->m_pluginName != 0;
}

MediaStreamTypes Effect::inputMediaStreamTypes() const
{
    return Phonon::Xine::Audio;
}

MediaStreamTypes Effect::outputMediaStreamTypes() const
{
    return Phonon::Xine::Audio;
}

QList<EffectParameter> Effect::parameters() const
{
    const_cast<Effect *>(this)->ensureParametersReady();
    K_XT(const Effect);
    return xt->m_parameterList;
}

void Effect::ensureParametersReady()
{
    K_XT(Effect);
    xt->ensureInstance();
}

QVariant Effect::parameterValue(const EffectParameter &p) const
{
    const int parameterIndex = p.id();
    K_XT(const Effect);
    QMutexLocker lock(&xt->m_mutex);
    if (!xt->m_plugin || !xt->m_pluginApi) {
        return QVariant(); // invalid
    }

    xine_post_api_descr_t *desc = xt->m_pluginApi->get_param_descr();
    Q_ASSERT(xt->m_pluginParams);
    xt->m_pluginApi->get_parameters(xt->m_plugin, xt->m_pluginParams);
    int i = 0;
    for (; i < parameterIndex && desc->parameter[i].type != POST_PARAM_TYPE_LAST; ++i);
    if (i == parameterIndex) {
        xine_post_api_parameter_t &p = desc->parameter[i];
        switch (p.type) {
        case POST_PARAM_TYPE_INT:          /* integer (or vector of integers)    */
            return *reinterpret_cast<int *>(xt->m_pluginParams + p.offset);
        case POST_PARAM_TYPE_DOUBLE:       /* double (or vector of doubles)      */
            return *reinterpret_cast<double *>(xt->m_pluginParams + p.offset);
        case POST_PARAM_TYPE_CHAR:         /* char (or vector of chars = string) */
        case POST_PARAM_TYPE_STRING:       /* (char *), ASCIIZ                   */
        case POST_PARAM_TYPE_STRINGLIST:   /* (char **) list, NULL terminated    */
            kWarning(610) << "char/string/stringlist parameter '" << (p.description ? p.description : p.name) << "' not supported.";
            return QVariant();
        case POST_PARAM_TYPE_BOOL:         /* integer (0 or 1)                   */
            return static_cast<bool>(*reinterpret_cast<int *>(xt->m_pluginParams + p.offset));
        case POST_PARAM_TYPE_LAST:         /* terminator of parameter list       */
            break;
        default:
            abort();
        }
    }
    kError(610) << "invalid parameterIndex passed to Effect::value";
    return QVariant();
}

void Effect::setParameterValue(const EffectParameter &p, const QVariant &newValue)
{
    K_XT(Effect);
    const int parameterIndex = p.id();
    QMutexLocker lock(&xt->m_mutex);
    if (!xt->m_plugin || !xt->m_pluginApi) {
        return;
    }

    xine_post_api_descr_t *desc = xt->m_pluginApi->get_param_descr();
    Q_ASSERT(xt->m_pluginParams);
    int i = 0;
    for (; i < parameterIndex && desc->parameter[i].type != POST_PARAM_TYPE_LAST; ++i);
    if (i == parameterIndex) {
        xine_post_api_parameter_t &p = desc->parameter[i];
        switch (p.type) {
        case POST_PARAM_TYPE_INT:          /* integer (or vector of integers)    */
            if (p.enum_values && newValue.type() == QVariant::String) {
                // need to convert to index
                int *value = reinterpret_cast<int *>(xt->m_pluginParams + p.offset);
                const QString string = newValue.toString();
                for (int j = 0; p.enum_values[j]; ++j) {
                    if (string == QString::fromUtf8(p.enum_values[j])) {
                        *value = j;
                        break;
                    }
                }
            } else {
                int *value = reinterpret_cast<int *>(xt->m_pluginParams + p.offset);
                *value = newValue.toInt();
            }
            break;
        case POST_PARAM_TYPE_DOUBLE:       /* double (or vector of doubles)      */
            {
                double *value = reinterpret_cast<double *>(xt->m_pluginParams + p.offset);
                *value = newValue.toDouble();
            }
            break;
        case POST_PARAM_TYPE_CHAR:         /* char (or vector of chars = string) */
        case POST_PARAM_TYPE_STRING:       /* (char *), ASCIIZ                   */
        case POST_PARAM_TYPE_STRINGLIST:   /* (char **) list, NULL terminated    */
            kWarning(610) << "char/string/stringlist parameter '" << (p.description ? p.description : p.name) << "' not supported.";
            return;
        case POST_PARAM_TYPE_BOOL:         /* integer (0 or 1)                   */
            {
               int *value = reinterpret_cast<int *>(xt->m_pluginParams + p.offset);
               *value = newValue.toBool() ? 1 : 0;
            }
            break;
        case POST_PARAM_TYPE_LAST:         /* terminator of parameter list       */
            kError(610) << "invalid parameterIndex passed to Effect::setValue";
            break;
        default:
            abort();
        }
        xt->m_pluginApi->set_parameters(xt->m_plugin, xt->m_pluginParams);
    } else {
        kError(610) << "invalid parameterIndex passed to Effect::setValue";
    }
}

void Effect::addParameter(const EffectParameter &p)
{
    K_XT(Effect);
    xt->m_parameterList << p;
}

void Effect::aboutToChangeXineEngine()
{
    K_XT(Effect);
    if (xt->m_plugin) {
        EffectXT *xt2 = new EffectXT(xt->m_pluginName);
        xt2->m_xine = xt->m_xine;
        xt2->m_plugin = xt->m_plugin;
        xt2->m_pluginApi = xt->m_pluginApi;
        xt2->m_fakeAudioPort = xt->m_fakeAudioPort;
        xt->m_plugin = 0;
        xt->m_pluginApi = 0;
        xt->m_fakeAudioPort = 0;
        KeepReference<> *keep = new KeepReference<>;
        keep->addObject(static_cast<SinkNodeXT *>(xt2));
        keep->ready();
    }
}

void Effect::xineEngineChanged()
{
    K_XT(Effect);
    if (!xt->m_xine) {
        xt->m_xine = Backend::xine();
    }
    //xt->createInstance();
}

}} //namespace Phonon::Xine

#include "moc_effect.cpp"
