/*  This file is part of the KDE project
    Copyright (C) 2010 Trever Fischer <tdfischer@fedoraproject.org>

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

#ifndef Phonon_GSTREAMER_VIDEODATAOUTPUT_H
#define Phonon_GSTREAMER_VIDEODATAOUTPUT_H

#include "medianode.h"
#include <phonon/experimental/abstractvideodataoutput.h>
#include <phonon/experimental/videodataoutputinterface.h>

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

namespace Phonon
{
namespace Gstreamer
{

class Backend;

    /**
     * \author Trever Fischer <tdfischer@fedoraproject.org>
     */
    class VideoDataOutput : public QObject,
                            public Experimental::VideoDataOutputInterface,
                            public MediaNode
    {
        Q_OBJECT
        Q_INTERFACES(Phonon::Experimental::VideoDataOutputInterface Phonon::Gstreamer::MediaNode);

    public:
        VideoDataOutput(Backend *, QObject *);
        ~VideoDataOutput();

        static void processBuffer(GstPad*, GstBuffer*, gpointer);

        Phonon::Experimental::AbstractVideoDataOutput *frontendObject() const { return m_frontend; }
        void setFrontendObject(Phonon::Experimental::AbstractVideoDataOutput *object) { m_frontend = object; }

        GstElement *videoElement() { return m_queue; }

        void mediaNodeEvent(const MediaNodeEvent *event);

    private:
        GstElement *m_queue;
        Phonon::Experimental::AbstractVideoDataOutput *m_frontend;
    };

}} //namespace Phonon::Gstreamer

QT_END_NAMESPACE
QT_END_HEADER

// vim: sw=4 ts=4 tw=80
#endif // Phonon_GSTREAMER_VIDEODATAOUTPUT_H
