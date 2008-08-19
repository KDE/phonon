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

#include <kglobal.h>

#include "nullsink.h"
#include "xineengine.h"
#include "sourcenode.h"
#include "xinestream.h"
#include "events.h"

namespace Phonon
{
namespace Xine
{

void NullSinkXT::rewireTo(SourceNodeXT *source)
{
    xine_post_out_t *audioSource = source->audioOutputPort();
    xine_post_out_t *videoSource = source->videoOutputPort();
    if (audioSource) {
        xine_post_wire_audio_port(audioSource, audioPort());
    }
    if (videoSource) {
        xine_post_wire_video_port(videoSource, videoPort());
    }
}

xine_audio_port_t *NullSinkXT::audioPort() const
{
    if (!m_stream) {
        return 0;
    }
    return m_stream->nullAudioPort();
}

xine_video_port_t *NullSinkXT::videoPort() const
{
    if (!m_stream) {
        return 0;
    }
    return m_stream->nullVideoPort();
}

void NullSink::downstreamEvent(Event *e)
{
    if (e->type() == Event::HeresYourXineStream) {
        K_XT(NullSink);
        xt->m_stream = static_cast<HeresYourXineStreamEvent *>(e)->stream.data();
    }
    SinkNode::downstreamEvent(e);
}

} // namespace Xine
} // namespace Phonon

#include "moc_nullsink.cpp"
#include "xineengine.h"
