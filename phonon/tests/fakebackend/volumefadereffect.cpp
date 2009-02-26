/*  This file is part of the KDE project
    Copyright (C) 2006 Matthias Kretz <kretz@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), Nokia Corporation 
    (or its successors, if any) and the KDE Free Qt Foundation, which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public 
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "volumefadereffect.h"
#include <QtCore/QVector>

namespace Phonon
{
namespace Fake
{
static const int VOLUMEFADEREFFECT_SAMPLE_RATE = 44100;

VolumeFaderEffect::VolumeFaderEffect(QObject *parent)
    : Effect(-1, parent), m_fadeTime(0)
{
}

VolumeFaderEffect::~VolumeFaderEffect()
{
}

float VolumeFaderEffect::volume() const
{
    if (m_fadeTime > 0)
    {
        float v = m_volume + (m_endvolume - m_volume) / m_fadeTime * m_fadeStart.elapsed();
        if ((m_volume < m_endvolume && v > m_endvolume) || (m_volume > m_endvolume && v < m_endvolume))
        {
            VolumeFaderEffect *t = const_cast<VolumeFaderEffect *>(this);
            t->m_volume = m_endvolume;
            t->m_fadeTime = 0;
        }
        else
            return v;
    }
    return m_volume;
}

void VolumeFaderEffect::setVolume(float volume)
{
    m_volume = volume;
}

Phonon::VolumeFaderEffect::FadeCurve VolumeFaderEffect::fadeCurve() const
{
    return m_fadeCurve;
}

void VolumeFaderEffect::setFadeCurve(Phonon::VolumeFaderEffect::FadeCurve curve)
{
    m_fadeCurve = curve;
}

void VolumeFaderEffect::fadeTo(float volume, int fadeTime)
{
    if (m_fadeTime > 0)
        m_volume = this->volume();
    m_fadeStart.start();
    m_fadeTime = fadeTime;
    m_endvolume = volume;
    m_fadePosition = 0;
    m_fadeLength = fadeTime * VOLUMEFADEREFFECT_SAMPLE_RATE;
}

void VolumeFaderEffect::processBuffer(QVector<float> &buffer)
{
    // ignore the fade curve and do a linear fade

    int bufferPos = 0;
    while (m_fadePosition < m_fadeLength) {
        if (bufferPos > buffer.size()) {
            return;
        }
        buffer[bufferPos++] *= ++m_fadePosition * (m_endvolume - m_volume) / m_fadeLength + m_volume;
    }
}

}} //namespace Phonon::Fake

#include "moc_volumefadereffect.cpp"
// vim: sw=4 ts=4
