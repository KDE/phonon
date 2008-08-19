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

#include "volumefadereffect.h"
#include "xineengine.h"
#include <kdebug.h>
#include <klocale.h>

namespace Phonon
{
namespace Xine
{

enum ParameterIds {
    VolumeParameter = 0,
    FadeCurveParameter = 1,
    FadeToParameter = 2,
    FadeTimeParameter = 3,
    StartFadeParameter = 4
};

VolumeFaderEffectXT::VolumeFaderEffectXT()
    : EffectXT("KVolumeFader"),
    m_parameters(Phonon::VolumeFaderEffect::Fade3Decibel, 1.0f, 1.0f, 0)
{
}

VolumeFaderEffect::VolumeFaderEffect(QObject *parent)
    : Effect(new VolumeFaderEffectXT, parent)
{
    const QVariant one = 1.0;
    const QVariant dZero = 0.0;
    const QVariant iZero = 0;
    addParameter(EffectParameter(VolumeParameter, i18n("Volume"), 0, one, dZero, one));
    addParameter(EffectParameter(FadeCurveParameter, i18n("Fade Curve"),
                EffectParameter::IntegerHint, iZero, iZero, 3));
    addParameter(EffectParameter(FadeToParameter, i18n("Fade To Volume"), 0, one, dZero, one));
    addParameter(EffectParameter(FadeTimeParameter, i18n("Fade Time"),
                EffectParameter::IntegerHint, iZero, iZero, 10000));
    addParameter(EffectParameter(StartFadeParameter, i18n("Start Fade"),
                EffectParameter::ToggledHint, iZero, iZero, 1));
}

VolumeFaderEffect::~VolumeFaderEffect()
{
}

QVariant VolumeFaderEffect::parameterValue(const EffectParameter &p) const
{
    K_XT(const VolumeFaderEffect);

    const int parameterId = p.id();
    kDebug(610) << parameterId;
    switch (static_cast<ParameterIds>(parameterId)) {
    case VolumeParameter:
        return static_cast<double>(volume());
    case FadeCurveParameter:
        return static_cast<int>(fadeCurve());
    case FadeToParameter:
        return static_cast<double>(xt->m_parameters.fadeTo);
    case FadeTimeParameter:
        return xt->m_parameters.fadeTime;
    case StartFadeParameter:
        return 0;
    }
    kError(610) << "request for unknown parameter " << parameterId;
    return QVariant();
}

void VolumeFaderEffect::setParameterValue(const EffectParameter &p, const QVariant &newValue)
{
    K_XT(VolumeFaderEffect);

    const int parameterId = p.id();
    kDebug(610) << parameterId << newValue;
    switch (static_cast<ParameterIds>(parameterId)) {
    case VolumeParameter:
        setVolume(newValue.toDouble());
        break;
    case FadeCurveParameter:
        setFadeCurve(static_cast<Phonon::VolumeFaderEffect::FadeCurve>(newValue.toInt()));
        break;
    case FadeToParameter:
        xt->m_parameters.fadeTo = newValue.toDouble();
        break;
    case FadeTimeParameter:
        xt->m_parameters.fadeTime = newValue.toInt();
        break;
    case StartFadeParameter:
        if (newValue.toBool()) {
            fadeTo(xt->m_parameters.fadeTo, xt->m_parameters.fadeTime);
        }
        break;
    default:
        kError(610) << "request for unknown parameter " << parameterId;
        break;
    }
}

void VolumeFaderEffectXT::createInstance()
{
    xine_audio_port_t *audioPort = fakeAudioPort();
    Q_ASSERT(0 == m_plugin);
    kDebug(610) << audioPort << " fadeTime = " << m_parameters.fadeTime;
    m_plugin = xine_post_init(m_xine, "KVolumeFader", 1, &audioPort, 0);
    xine_post_in_t *paraInput = xine_post_input(m_plugin, "parameters");
    Q_ASSERT(paraInput);
    Q_ASSERT(paraInput->type == XINE_POST_DATA_PARAMETERS);
    Q_ASSERT(paraInput->data);
    m_pluginApi = reinterpret_cast<xine_post_api_t *>(paraInput->data);
    m_pluginApi->set_parameters(m_plugin, &m_parameters);
}

void VolumeFaderEffect::getParameters() const
{
    K_XT(const VolumeFaderEffect);

    if (xt->m_pluginApi) {
        xt->m_pluginApi->get_parameters(xt->m_plugin, &xt->m_parameters);
    }
}

float VolumeFaderEffect::volume() const
{
    K_XT(const VolumeFaderEffect);

    //kDebug(610);
    getParameters();
    return xt->m_parameters.currentVolume;
}

void VolumeFaderEffect::setVolume(float volume)
{
    K_XT(VolumeFaderEffect);

    //kDebug(610) << volume;
    xt->m_parameters.currentVolume = volume;
}

Phonon::VolumeFaderEffect::FadeCurve VolumeFaderEffect::fadeCurve() const
{
    K_XT(const VolumeFaderEffect);

    //kDebug(610);
    getParameters();
    return xt->m_parameters.fadeCurve;
}

void VolumeFaderEffect::setFadeCurve(Phonon::VolumeFaderEffect::FadeCurve curve)
{
    K_XT(VolumeFaderEffect);

    //kDebug(610) << curve;
    xt->m_parameters.fadeCurve = curve;
}

void VolumeFaderEffect::fadeTo(float volume, int fadeTime)
{
    K_XT(VolumeFaderEffect);

    //kDebug(610) << volume << fadeTime;
    xt->m_parameters.fadeTo = volume;
    xt->m_parameters.fadeTime = fadeTime;
    if (xt->m_pluginApi) {
        xt->m_pluginApi->set_parameters(xt->m_plugin, &xt->m_parameters);
    }
}

void VolumeFaderEffectXT::rewireTo(SourceNodeXT *source)
{
    if (!source->audioOutputPort()) {
        return;
    }
    EffectXT::rewireTo(source);
    Q_ASSERT(m_pluginApi);
    Q_ASSERT(m_plugin);
    m_pluginApi->set_parameters(m_plugin, &m_parameters);
}

}} //namespace Phonon::Xine

#include "volumefadereffect.moc"
// vim: sw=4 ts=4
