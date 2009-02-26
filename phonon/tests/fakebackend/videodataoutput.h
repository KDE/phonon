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
#ifndef Phonon_FAKE_VIDEODATAOUTPUT_H
#define Phonon_FAKE_VIDEODATAOUTPUT_H

#include "abstractvideooutput.h"
#include <phonon/experimental/videoframe.h>
#include <QtCore/QVector>
#include <QtCore/QByteRef>
#include <QtCore/QObject>
#include <QtCore/QSize>

namespace Phonon
{
namespace Fake
{
    /**
     * \author Matthias Kretz <kretz@kde.org>
     */
    class VideoDataOutput : public QObject, public AbstractVideoOutput
    {
        Q_OBJECT
        Q_INTERFACES(Phonon::Fake::AbstractVideoOutput)
        public:
            VideoDataOutput(QObject *parent);
            ~VideoDataOutput();

        public Q_SLOTS:
            int frameRate() const;
            void setFrameRate(int frameRate);

            QSize naturalFrameSize() const;
            QSize frameSize() const;
            void setFrameSize(const QSize &frameSize);

            //int displayLatency() const;
            //void setDisplayLatency(int milliseconds);

        public:
            // Fake specific:
            virtual void processFrame(Phonon::Experimental::VideoFrame &frame);

        signals:
            void frameReady(const Phonon::Experimental::VideoFrame &frame);
            void endOfMedia();

        private:
            QByteArray m_pendingData;
            //int m_latency;
            int m_frameRate;
            QSize m_frameSize;
    };
}} //namespace Phonon::Fake

// vim: sw=4 ts=4 tw=80
#endif // Phonon_FAKE_VIDEODATAOUTPUT_H
