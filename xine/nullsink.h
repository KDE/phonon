/*  This file is part of the KDE project
    Copyright (C) 2007 Matthias Kretz <kretz@kde.org>

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

#ifndef NULLSINK_H
#define NULLSINK_H

#include "sinknode.h"
#include <QtCore/QObject>

namespace Phonon
{
namespace Xine
{

class XineStream;
class NullSinkXT : public SinkNodeXT
{
    public:
        NullSinkXT() : SinkNodeXT("NullSink"), m_stream(0) {}
        virtual void rewireTo(SourceNodeXT *);
        virtual xine_audio_port_t *audioPort() const;
        virtual xine_video_port_t *videoPort() const;

    private:
        friend class NullSink;
        QPointer<XineStream> m_stream;
};

class NullSinkPrivate;
class NullSink : public QObject, public SinkNode
{
    friend class NullSinkPrivate;
    Q_OBJECT
    public:
        NullSink(QObject *parent) : QObject(parent), SinkNode(new NullSinkXT) {}
        MediaStreamTypes inputMediaStreamTypes() const { return Phonon::Xine::Audio | Phonon::Xine::Video; }
        void downstreamEvent(Event *e);
};
} // namespace Xine
} // namespace Phonon
#endif // NULLSINK_H
