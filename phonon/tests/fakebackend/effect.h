/*  This file is part of the KDE project
    Copyright (C) 2006-2007 Matthias Kretz <kretz@kde.org>

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
#ifndef Phonon_FAKE_AUDIOEFFECT_H
#define Phonon_FAKE_AUDIOEFFECT_H

#include <QtCore/QObject>
#include <phonon/effectparameter.h>
#include <phonon/effectinterface.h>
#include "audionode.h"

namespace Phonon
{
namespace Fake
{
    class EffectInterface;

    class Effect : public QObject, public Phonon::EffectInterface, public AudioNode
    {
        Q_OBJECT
        Q_INTERFACES(Phonon::EffectInterface Phonon::Fake::AudioNode)
        public:
            Effect(int effectId, QObject *parent);
            ~Effect();

            QList<EffectParameter> parameters() const;
            QVariant parameterValue(const EffectParameter &) const;
            void setParameterValue(const EffectParameter &, const QVariant &);

            // Fake specific:
            virtual void processBuffer(QVector<float> &buffer);

            bool setAudioSink(AudioNode *node);

        private:
            AudioNode *m_sink;
            Phonon::Fake::EffectInterface *m_effect;
            QList<Phonon::EffectParameter> m_parameterList;
    };
}} //namespace Phonon::Fake

// vim: sw=4 ts=4 tw=80
#endif // Phonon_FAKE_AUDIOEFFECT_H
