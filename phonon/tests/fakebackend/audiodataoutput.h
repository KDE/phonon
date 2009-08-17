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
#ifndef Phonon_FAKE_AUDIODATAOUTPUT_H
#define Phonon_FAKE_AUDIODATAOUTPUT_H

#include "abstractaudiooutput.h"
#include <phonon/audiodataoutput.h>
#include <QtCore/QVector>

namespace Phonon
{
namespace Fake
{
    /**
     * \author Matthias Kretz <kretz@kde.org>
     */
    class AudioDataOutput : public AbstractAudioOutput
    {
        Q_OBJECT
        public:
            AudioDataOutput(QObject *parent);
            ~AudioDataOutput();

        public Q_SLOTS:
            int dataSize() const;
            int sampleRate() const;
            void setDataSize(int size);

        public:
            // Fake specific:
            virtual void processBuffer(QVector<float> &buffer);

        signals:
            void dataReady(const QMap<Phonon::AudioDataOutput::Channel, QVector<qint16> > &data);
            void endOfMedia(int remainingSamples);

        private:
            void convertAndEmit(const QVector<float> &buffer);

            int m_dataSize;
            QVector<float> m_pendingData;
    };
}} //namespace Phonon::Fake

// vim: sw=4 ts=4 tw=80
#endif // Phonon_FAKE_AUDIODATAOUTPUT_H
