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

#include "videoeffect.h"
#include <QVariant>
#include "xineengine.h"
#include <QMutexLocker>

namespace Phonon
{
namespace Xine
{
VideoEffect::VideoEffect(int effectId, QObject *parent)
    : QObject(parent),
    m_pluginName(0),
    m_pluginParams(0),
    m_path(0)
{
    const char *const *postPlugins = xine_list_post_plugins_typed(m_xine, XINE_POST_TYPE_VIDEO_FILTER);
    if (effectId >= 0x7F000000) {
        effectId -= 0x7F000000;
        for(int i = 0; postPlugins[i]; ++i) {
            if (i == effectId) {
                // found it
                m_pluginName = postPlugins[i];
                break;
            }
        }
    }
}

VideoEffect::VideoEffect(const char *name, QObject *parent)
    : QObject(parent),
    m_pluginName(name),
    m_pluginParams(0),
    m_path(0)
{
}

VideoEffect::~VideoEffect()
{
    foreach (xine_post_t *post, m_plugins) {
        xine_post_dispose(m_xine, post);
    }
    free(m_pluginParams);
}

void VideoEffect::setPath(VideoPath *path)
{
    m_path = path;
}

QList<EffectParameter> VideoEffect::allDescriptions()
{
    ensureParametersReady();
    return m_parameterList;
}

EffectParameter VideoEffect::description(int parameterIndex)
{
    ensureParametersReady();
    if (parameterIndex >= m_parameterList.size()) {
        return EffectParameter();
    }
    return m_parameterList[parameterIndex];
}

int VideoEffect::parameterCount()
{
    ensureParametersReady();
    return m_parameterList.size();
}

void VideoEffect::ensureParametersReady()
{
    if (m_parameterList.isEmpty() && m_plugins.isEmpty()) {
        newInstance(XineEngine::nullVideoPort());
        if (!m_plugins.isEmpty()) {
            xine_post_dispose(m_xine, m_plugins.first());
            m_plugins.clear();
        }
    }
}

xine_post_t *VideoEffect::newInstance(xine_video_port_t *videoPort)
{
    QMutexLocker lock(&m_mutex);
    if (m_pluginName) {
        xine_post_t *x = xine_post_init(m_xine, m_pluginName, 1, 0, &videoPort);
        m_plugins << x;
        xine_post_in_t *paraInput = xine_post_input(x, "parameters");
        if (paraInput) {
            Q_ASSERT(paraInput->type == XINE_POST_DATA_PARAMETERS);
            Q_ASSERT(paraInput->data);
            xine_post_api_t *api = reinterpret_cast<xine_post_api_t *>(paraInput->data);
            m_pluginApis << api;
            if (m_parameterList.isEmpty()) {
                xine_post_api_descr_t *desc = api->get_param_descr();
                Q_ASSERT(0 == m_pluginParams);
                m_pluginParams = static_cast<char *>(malloc(desc->struct_size));
                api->get_parameters(x, m_pluginParams);
                for (int i = 0; desc->parameter[i].type != POST_PARAM_TYPE_LAST; ++i) {
                    xine_post_api_parameter_t &p = desc->parameter[i];
                    switch (p.type) {
                    case POST_PARAM_TYPE_INT:          /* integer (or vector of integers)    */
                        addParameter(EffectParameter(i, p.name, EffectParameter::IntegerHint,
                                    *reinterpret_cast<int *>(m_pluginParams + p.offset),
                                    static_cast<int>(p.range_min), static_cast<int>(p.range_max), p.description));
                        break;
                    case POST_PARAM_TYPE_DOUBLE:       /* double (or vector of doubles)      */
                        addParameter(EffectParameter(i, p.name, 0,
                                    *reinterpret_cast<double *>(m_pluginParams + p.offset),
                                    p.range_min, p.range_max, p.description));
                        break;
                    case POST_PARAM_TYPE_CHAR:         /* char (or vector of chars = string) */
                    case POST_PARAM_TYPE_STRING:       /* (char *), ASCIIZ                   */
                    case POST_PARAM_TYPE_STRINGLIST:   /* (char **) list, NULL terminated    */
                        kWarning(610) << "char/string/stringlist parameter '" << p.name << "' not supported.";
                        break;
                    case POST_PARAM_TYPE_BOOL:         /* integer (0 or 1)                   */
                        addParameter(EffectParameter(i, p.name, EffectParameter::ToggledHint,
                                    static_cast<bool>(*reinterpret_cast<int *>(m_pluginParams + p.offset)),
                                    QVariant(), QVariant(), p.description));
                        break;
                    case POST_PARAM_TYPE_LAST:         /* terminator of parameter list       */
                    default:
                        abort();
                    }
                }
            }
        } else {
            m_pluginApis << 0;
        }
        return x;
    }
    return 0;
}

QVariant VideoEffect::parameterValue(int parameterIndex) const
{
    QMutexLocker lock(&m_mutex);
    if (m_plugins.isEmpty() || m_pluginApis.isEmpty()) {
        return QVariant(); // invalid
    }

    xine_post_t *post = m_plugins.first();
    xine_post_api_t *api = m_pluginApis.first();
    if (!post || !api) {
        return QVariant(); // invalid
    }

    xine_post_api_descr_t *desc = api->get_param_descr();
    Q_ASSERT(m_pluginParams);
    api->get_parameters(post, m_pluginParams);
    int i = 0;
    for (; i < parameterIndex && desc->parameter[i].type != POST_PARAM_TYPE_LAST; ++i);
    if (i == parameterIndex) {
        xine_post_api_parameter_t &p = desc->parameter[i];
        switch (p.type) {
        case POST_PARAM_TYPE_INT:          /* integer (or vector of integers)    */
            return *reinterpret_cast<int *>(m_pluginParams + p.offset);
        case POST_PARAM_TYPE_DOUBLE:       /* double (or vector of doubles)      */
            return *reinterpret_cast<double *>(m_pluginParams + p.offset);
        case POST_PARAM_TYPE_CHAR:         /* char (or vector of chars = string) */
        case POST_PARAM_TYPE_STRING:       /* (char *), ASCIIZ                   */
        case POST_PARAM_TYPE_STRINGLIST:   /* (char **) list, NULL terminated    */
            kWarning(610) << "char/string/stringlist parameter '" << p.name << "' not supported.";
            return QVariant();
        case POST_PARAM_TYPE_BOOL:         /* integer (0 or 1)                   */
            return static_cast<bool>(*reinterpret_cast<int *>(m_pluginParams + p.offset));
        case POST_PARAM_TYPE_LAST:         /* terminator of parameter list       */
            break;
        default:
            abort();
        }
    }
    kError(610) << "invalid parameterIndex passed to VideoEffect::value";
    return QVariant();
}

void VideoEffect::setParameterValue(int parameterIndex, const QVariant &newValue)
{
    QMutexLocker lock(&m_mutex);
    if (m_plugins.isEmpty() || m_pluginApis.isEmpty()) {
        return;
    }

    xine_post_t *post = m_plugins.first();
    xine_post_api_t *api = m_pluginApis.first();
    if (!post || !api) {
        return;
    }

    xine_post_api_descr_t *desc = api->get_param_descr();
    Q_ASSERT(m_pluginParams);
    int i = 0;
    for (; i < parameterIndex && desc->parameter[i].type != POST_PARAM_TYPE_LAST; ++i);
    if (i == parameterIndex) {
        xine_post_api_parameter_t &p = desc->parameter[i];
        switch (p.type) {
        case POST_PARAM_TYPE_INT:          /* integer (or vector of integers)    */
            {
                int *value = reinterpret_cast<int *>(m_pluginParams + p.offset);
                *value = newValue.toInt();
            }
            break;
        case POST_PARAM_TYPE_DOUBLE:       /* double (or vector of doubles)      */
            {
                double *value = reinterpret_cast<double *>(m_pluginParams + p.offset);
                *value = newValue.toDouble();
            }
            break;
        case POST_PARAM_TYPE_CHAR:         /* char (or vector of chars = string) */
        case POST_PARAM_TYPE_STRING:       /* (char *), ASCIIZ                   */
        case POST_PARAM_TYPE_STRINGLIST:   /* (char **) list, NULL terminated    */
            kWarning(610) << "char/string/stringlist parameter '" << p.name << "' not supported.";
            return;
        case POST_PARAM_TYPE_BOOL:         /* integer (0 or 1)                   */
            {
               int *value = reinterpret_cast<int *>(m_pluginParams + p.offset);
               *value = newValue.toBool() ? 1 : 0;
            }
            break;
        case POST_PARAM_TYPE_LAST:         /* terminator of parameter list       */
            kError(610) << "invalid parameterIndex passed to VideoEffect::setValue";
            break;
        default:
            abort();
        }
        api->set_parameters(post, m_pluginParams);
    } else {
        kError(610) << "invalid parameterIndex passed to VideoEffect::setValue";
    }
}

}} //namespace Phonon::Xine

#include "videoeffect.moc"
// vim: sw=4 ts=4
