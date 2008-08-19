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

#include "videodataoutput.h"

#include "events.h"
#include "keepreference.h"
#include "sourcenode.h"
#include "wirecall.h"
#include "xinethread.h"

#include <QtCore/QCoreApplication>

#include <KDE/KDebug>

#include <Phonon/Experimental/AbstractVideoDataOutput>
#include <Phonon/Experimental/VideoFrame2>

namespace Phonon
{
namespace Xine
{
class VideoDataOutputXT : public SinkNodeXT
{
    public:
        VideoDataOutputXT();
        ~VideoDataOutputXT();
        xine_video_port_t *videoPort() const;
        void rewireTo(SourceNodeXT *);
        bool setFrontendObject(Experimental::AbstractVideoDataOutput *x);

        Phonon::Experimental::AbstractVideoDataOutput *m_frontend;
    private:
#ifdef XINE_VISUAL_TYPE_RAW
        static void raw_output_cb(void *user_data, int frame_format, int frame_width,
                int frame_height, double frame_aspect, void *data0, void *data1, void *data2);
        static void raw_overlay_cb(void *user_data, int num_ovl, raw_overlay_t *overlay_array);
        raw_visual_t m_visual;
#endif
        int m_supported_formats;
    public:
        bool m_needNewPort;
        xine_video_port_t *m_videoPort;
};

#ifdef XINE_VISUAL_TYPE_RAW
void VideoDataOutputXT::raw_output_cb(void *user_data, int format, int width,
        int height, double aspect, void *data0, void *data1, void *data2)
{
    kDebug(610);
    VideoDataOutputXT* vw = reinterpret_cast<VideoDataOutputXT *>(user_data);
    const Experimental::VideoFrame2 f = {
        width,
        height,
        aspect,
        ((format == XINE_VORAW_YV12) ? Experimental::VideoFrame2::Format_YV12 :
         (format == XINE_VORAW_YUY2) ? Experimental::VideoFrame2::Format_YUY2 :
         (format == XINE_VORAW_RGB ) ? Experimental::VideoFrame2::Format_RGB888 :
                                       Experimental::VideoFrame2::Format_Invalid),
        QByteArray::fromRawData(reinterpret_cast<const char *>(data0), ((format == XINE_VORAW_RGB) ? 3 : (format == XINE_VORAW_YUY2) ? 2 : 1) * width * height),
        QByteArray::fromRawData(reinterpret_cast<const char *>(data1), (format == XINE_VORAW_YV12) ? (width >> 1) + (height >> 1) : 0),
        QByteArray::fromRawData(reinterpret_cast<const char *>(data2), (format == XINE_VORAW_YV12) ? (width >> 1) + (height >> 1) : 0)
    };
    if (vw->m_frontend) {
        kDebug(610) << "calling frameReady on the frontend";
        vw->m_frontend->frameReady(f);
    }
}

void VideoDataOutputXT::raw_overlay_cb(void *user_data, int num_ovl, raw_overlay_t *overlay_array)
{
    VideoDataOutputXT* vw = reinterpret_cast<VideoDataOutputXT *>(user_data);
    Q_UNUSED(vw);
    Q_UNUSED(num_ovl);
    Q_UNUSED(overlay_array);
    // TODO do we want to handle overlays? How?
}
#endif

VideoDataOutputXT::VideoDataOutputXT()
    : m_frontend(0),
#ifdef XINE_VISUAL_TYPE_RAW
    m_supported_formats(XINE_VORAW_YV12 | XINE_VORAW_YUY2 | XINE_VORAW_RGB),
    m_needNewPort(true),
#endif
    m_videoPort(0)
{
    m_xine = Backend::xine();
}

VideoDataOutputXT::~VideoDataOutputXT()
{
    if (m_videoPort) {
        xine_video_port_t *vp = m_videoPort;
        m_videoPort = 0;

        if (vp) {
            xine_close_video_driver(m_xine, vp);
        }
    }
}

VideoDataOutput::VideoDataOutput(QObject *parent)
    : QObject(parent),
    SinkNode(new VideoDataOutputXT)
{
}

VideoDataOutput::~VideoDataOutput()
{
}

xine_video_port_t *VideoDataOutputXT::videoPort() const
{
#ifdef XINE_VISUAL_TYPE_RAW
    if (m_needNewPort) {
        VideoDataOutputXT *that = const_cast<VideoDataOutputXT *>(this);
        that->m_needNewPort = false;
        that->m_visual.user_data = static_cast<void *>(that);
        that->m_visual.supported_formats = m_supported_formats;
        that->m_visual.raw_output_cb = &Phonon::Xine::VideoDataOutputXT::raw_output_cb;
        that->m_visual.raw_overlay_cb = &Phonon::Xine::VideoDataOutputXT::raw_overlay_cb;
        kDebug(610) << "create new raw video port with supported_formats =" << that->m_visual.supported_formats;
        xine_video_port_t *newVideoPort = xine_open_video_driver(m_xine, "auto", XINE_VISUAL_TYPE_RAW, static_cast<void *>(&that->m_visual));
        if (m_videoPort) {
            // TODO delayed destruction of m_videoPort
        }
        that->m_videoPort = newVideoPort;
        return newVideoPort;
    }
#endif
    return m_videoPort;
}

void VideoDataOutputXT::rewireTo(SourceNodeXT *source)
{
    kDebug(610);
    if (!source->videoOutputPort()) {
        return;
    }
    kDebug(610) << "do something";
    xine_post_wire_video_port(source->videoOutputPort(), videoPort());
}

Experimental::AbstractVideoDataOutput *VideoDataOutput::frontendObject() const
{
    K_XT(const VideoDataOutput);
    return xt->m_frontend;
}

bool VideoDataOutputXT::setFrontendObject(Experimental::AbstractVideoDataOutput *x)
{
    m_frontend = x;
#ifdef XINE_VISUAL_TYPE_RAW
    if (m_frontend) {
        int supported_formats = 0;
        if (m_frontend->allowedFormats().contains(Experimental::VideoFrame2::Format_RGB888)) {
            supported_formats |= XINE_VORAW_RGB;
        }
        if (m_frontend->allowedFormats().contains(Experimental::VideoFrame2::Format_YV12)) {
            supported_formats |= XINE_VORAW_YV12;
        }
        if (m_frontend->allowedFormats().contains(Experimental::VideoFrame2::Format_YUY2)) {
            supported_formats |= XINE_VORAW_YUY2;
        }
        if (m_supported_formats != supported_formats) {
            m_supported_formats = supported_formats;
            m_needNewPort = true;
            return true;
        }
    }
#endif
    return false;
}

void VideoDataOutput::setFrontendObject(Experimental::AbstractVideoDataOutput *x)
{
    K_XT(VideoDataOutput);
    if (xt->setFrontendObject(x)) {
        // we need to recreate and rewire the output
        SourceNode *src = source();
        if (src) {
            QList<WireCall> wireCall;
            wireCall << WireCall(src, this);
            QCoreApplication::postEvent(XineThread::instance(), new RewireEvent(wireCall, QList<WireCall>()));
        }
    }
}

void VideoDataOutput::aboutToChangeXineEngine()
{
    kDebug();
    K_XT(VideoDataOutput);
    if (xt->m_videoPort) {
        VideoDataOutputXT *xt2 = new VideoDataOutputXT();
        xt2->m_xine = xt->m_xine;
        xt2->m_videoPort = xt->m_videoPort;
        xt2->m_needNewPort = false;
        xt->m_needNewPort = true;
        xt->m_videoPort = 0;
        KeepReference<> *keep = new KeepReference<>;
        keep->addObject(xt2);
        keep->ready();
    }
}

void VideoDataOutput::xineEngineChanged()
{
//X     kDebug();
//X     K_XT(VideoDataOutput);
//X     if (xt->m_xine) {
//X         Q_ASSERT(!xt->m_videoPort);
//X         xt->createVideoPort();
//X     }
}

}} //namespace Phonon::Xine

#include "videodataoutput.moc"
