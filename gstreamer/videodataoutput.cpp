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

#include "videodataoutput.h"
#include <phonon/experimental/videoframe2.h>
#include "mediaobject.h"

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

namespace Phonon
{
namespace Gstreamer
{
    
VideoDataOutput::VideoDataOutput(Backend *backend, QObject *parent)
    : QObject(parent),
    MediaNode(backend, VideoSink | VideoSource)
{
    static int count = 0;
    m_name = "VideoDataOutput" + QString::number(count++);
    
    m_queue = gst_bin_new(NULL);
    gst_object_ref(GST_OBJECT(m_queue));
    gst_object_sink(GST_OBJECT(m_queue));
    
    GstElement* input = gst_element_factory_make("queue", NULL);
    GstElement* output = gst_element_factory_make("identity", NULL);
    GstElement* convert = gst_element_factory_make("ffmpegcolorspace", NULL);
    
	// Save ourselves a metric crapton of work by simply requesting
	// a format native to Qt.
    GstCaps *caps = gst_caps_new_simple("video/x-raw-rgb",
                                        "bpp", G_TYPE_INT, 24,
                                        "depth", G_TYPE_INT, 24,
                                        NULL);
    
    
    gst_bin_add_many(GST_BIN(m_queue), input, convert, output, NULL);
    gst_element_link_filtered(convert, output, caps);
    gst_caps_unref(caps);
    gst_element_link(input, convert);
    
    GstPad *outputpad = gst_element_get_pad(output, "src");
    gst_pad_add_buffer_probe(outputpad, G_CALLBACK(processBuffer), this);
    gst_element_add_pad(m_queue, gst_ghost_pad_new("src", outputpad));
    gst_object_unref(outputpad);
    
    GstPad *inputpad = gst_element_get_pad(input, "sink");
    gst_element_add_pad(m_queue, gst_ghost_pad_new("sink", inputpad));
    gst_object_unref(inputpad);
    m_isValid = true;
}

VideoDataOutput::~VideoDataOutput()
{
    gst_element_set_state(m_queue, GST_STATE_NULL);
    gst_object_unref(m_queue);
}

void VideoDataOutput::processBuffer(GstPad*, GstBuffer* buffer, gpointer gThat)
{
    VideoDataOutput *that = reinterpret_cast<VideoDataOutput*>(gThat);
    
    GstStructure* structure = gst_caps_get_structure(GST_BUFFER_CAPS(buffer), 0);
    int width;
    int height;
    double aspect;
    
    gst_structure_get_int(structure, "width", &width);
    gst_structure_get_int(structure, "height", &height);
    aspect = (double)width/height;
    const Experimental::VideoFrame2 f = {
        width,
        height,
        aspect,
        Experimental::VideoFrame2::Format_RGB888,
		// RGB888 Means the data is 8 bits o' red, 8 bits o' green, and 8 bits o' blue per pixel.
        QByteArray::fromRawData(reinterpret_cast<const char*>(GST_BUFFER_DATA(buffer)), 3*width*height),
        0,
        0
    };
    if (that->m_frontend) {
        that->m_frontend->frameReady(f);
    }
}

void VideoDataOutput::mediaNodeEvent(const MediaNodeEvent *event)
{
    if (event->type() == MediaNodeEvent::MediaObjectConnected && root()) {
        g_object_set(G_OBJECT(videoElement()), "sync", true, (const char*)NULL);
        return;
    }
    MediaNode::mediaNodeEvent(event);
}

}} // namespace Phonon::Gstreamer

QT_END_NAMESPACE
QT_END_HEADER

#include "moc_videodataoutput.cpp"
// vim: sw=4 ts=4
