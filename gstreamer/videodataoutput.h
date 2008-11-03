/*  This file is part of the KDE project
    Copyright (C) 2008 Matthias Kretz <kretz@kde.org>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) version 3.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.

*/

#ifndef PHONON_GSTREAMER_VIDEODATAOUTPUT_H
#define PHONON_GSTREAMER_VIDEODATAOUTPUT_H

#include <Phonon/Experimental/VideoDataOutputInterface>
#include "medianode.h"

QT_BEGIN_NAMESPACE

namespace Phonon
{
namespace Gstreamer
{

class VideoDataOutput : public QObject, public Phonon::Experimental::VideoDataOutputInterface, public MediaNode
{
    Q_OBJECT
    Q_INTERFACES(Phonon::Experimental::VideoDataOutputInterface Phonon::Gstreamer::MediaNode)
    public:
        VideoDataOutput(Backend *backend, QObject *parent = 0);
        ~VideoDataOutput();

        virtual AbstractVideoDataOutput *frontendObject() const;
        virtual void setFrontendObject(AbstractVideoDataOutput *);

    private:
        AbstractVideoDataOutput *m_frontendObject;
};

} // namespace GStreamer
} // namespace Phonon

QT_END_NAMESPACE

#endif // PHONON_GSTREAMER_VIDEODATAOUTPUT_H
