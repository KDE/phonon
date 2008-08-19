/*  This file is part of the KDE project
    Copyright (C) 2006,2008 Matthias Kretz <kretz@kde.org>

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
#ifndef PHONON_XINE_VIDEODATAOUTPUT_H
#define PHONON_XINE_VIDEODATAOUTPUT_H

#include "sinknode.h"

#include <Phonon/Experimental/VideoDataOutputInterface>
#include <Phonon/Experimental/VideoFrame>

#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtCore/QSize>
#include <QtCore/QVector>

#include <xine.h>

namespace Phonon
{
namespace Xine
{

class VideoDataOutput : public QObject, public Phonon::Experimental::VideoDataOutputInterface, public Phonon::Xine::SinkNode
{
    Q_OBJECT
    Q_INTERFACES(Phonon::Experimental::VideoDataOutputInterface Phonon::Xine::SinkNode)
    public:
        VideoDataOutput(QObject *parent);
        ~VideoDataOutput();

        MediaStreamTypes inputMediaStreamTypes() const { return Phonon::Xine::Video; }

        Experimental::AbstractVideoDataOutput *frontendObject() const;
        void setFrontendObject(Experimental::AbstractVideoDataOutput *);

    protected:
        void aboutToChangeXineEngine();
        void xineEngineChanged();
};
}} //namespace Phonon::Xine

#endif // PHONON_XINE_VIDEODATAOUTPUT_H
