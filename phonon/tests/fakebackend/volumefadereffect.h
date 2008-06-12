/*  This file is part of the KDE project
    Copyright (C) 2006 Matthias Kretz <kretz@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), Trolltech ASA 
    (or its successors, if any) and the KDE Free Qt Foundation, which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public 
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.

*/
#ifndef Phonon_FAKE_VOLUMEFADEREFFECT_H
#define Phonon_FAKE_VOLUMEFADEREFFECT_H

#include <QtCore/QDate>
#include "effect.h"
#include <phonon/volumefadereffect.h>
#include <phonon/volumefaderinterface.h>

namespace Phonon
{
namespace Fake
{
    /**
     * \author Matthias Kretz <kretz@kde.org>
     */
    class VolumeFaderEffect : public Effect, public Phonon::VolumeFaderInterface
    {
        Q_OBJECT
        Q_INTERFACES(Phonon::VolumeFaderInterface)

        public:
            VolumeFaderEffect(QObject *parent);
            ~VolumeFaderEffect();

            float volume() const;
            void setVolume(float volume);
            Phonon::VolumeFaderEffect::FadeCurve fadeCurve() const;
            void setFadeCurve(Phonon::VolumeFaderEffect::FadeCurve curve);
            void fadeTo(float volume, int fadeTime);

            void processBuffer(QVector<float> &buffer);

        private:
            float m_volume;
            float m_endvolume;
            int m_fadePosition;
            int m_fadeLength;
            int m_fadeTime;
            QTime m_fadeStart;
            Phonon::VolumeFaderEffect::FadeCurve m_fadeCurve;
    };
}} //namespace Phonon::Fake

// vim: sw=4 ts=4 tw=80
#endif // Phonon_FAKE_VOLUMEFADEREFFECT_H
