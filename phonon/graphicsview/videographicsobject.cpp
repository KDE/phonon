/*
    Copyright (C) 2011-2012 Harald Sitter <sitter@kde.org>

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

#include "videographicsobject.h"

#include "abstractvideographicspainter.h"
#include "factory_p.h"
#include "glslpainter.h"
#include "medianode_p.h"
#include "phonondefs_p.h"
#include "qpainterpainter.h"
#include "videoframe.h"
#include "videographicsobjectinterface.h"

#define PHONON_INTERFACENAME VideoGraphicsObjectInterface

namespace Phonon {

QMap<GraphicsPainterType, QList<VideoFrame::Format> > VideoGraphicsPainterMetaFactory::detectTypes()
{
    QMap<GraphicsPainterType, QList<VideoFrame::Format> > map;
    map.insert(GraphicsPainterGlsl, GlslPainter().supportedFormats());
    map.insert(GraphicsPainterQPainter, QPainterPainter().supportedFormats());
    return map;
}

class PrivatePainterFactory
{
public:
    AbstractVideoGraphicsPainter *factorize(QList<VideoFrame::Format> choices,
                                            QMap<GraphicsPainterType, QList<VideoFrame::Format> > formats)
    {
        // Try to find a matching format.
        // Iterate the painters in order glsl>glarb>qpainter, foreach painter check
        // if it supports one of the chosen formats in the order from the backend.
        // That way we get to choose our most preferred (e.g. reliable) painter,
        // while honoring the backends priorization. Painting is weighted more
        // because a bad painter in Qt Quick 1 will cause insanely bad quality (QPainter...)
        GraphicsPainterType painterType = GraphicsPainterNone;
        QMap<GraphicsPainterType,QList<VideoFrame::Format> >::const_iterator it = formats.constBegin();
        while (it != formats.constEnd()) {
            foreach (VideoFrame::Format format, choices) {
                if (it.value().contains(format)) {
                    chosenFormat = format;
                    painterType = it.key();
                    break;
                }
            }
            if (painterType != GraphicsPainterNone)
                break;
            ++it;
        }

        switch(painterType) {
        case GraphicsPainterNone:
            // TODO: what to do what to do? :(
            Q_ASSERT(painterType != GraphicsPainterNone);
        case GraphicsPainterGlsl:
            return new GlslPainter;
        case GraphicsPainterQPainter:
            return new QPainterPainter;
        }

        // Cannot reach.
        Q_ASSERT(false);
        return 0;
    }

    VideoFrame::Format chosenFormat;
};

// --------------------------------- PRIVATE -------------------------------- //
class VideoGraphicsObjectPrivate : public MediaNodePrivate
{
    P_DECLARE_PUBLIC(VideoGraphicsObject)
public:
    VideoGraphicsObjectPrivate() :
        geometry(0, 0, 320, 240),
        boundingRect(0, 0, 0, 0),
        frameSize(0, 0),
        graphicsPainter(0),
        paintedOnce(false),
        gotSize(false),
        ready(false)
    {}

    virtual ~VideoGraphicsObjectPrivate()
    {
        if (graphicsPainter)
            delete graphicsPainter;
    }

    virtual QObject *qObject() { return q_func(); }

    void updateBoundingRect();
    void _p_negotiateFormat();

    QRectF geometry;
    QRectF boundingRect;
    QSize frameSize;

    AbstractVideoGraphicsPainter *graphicsPainter;

    // FIXME: pfui
    QMap<GraphicsPainterType,QList<VideoFrame::Format> > spyFormats;

    bool paintedOnce;
    bool gotSize;
    bool ready;

protected:
    bool aboutToDeleteBackendObject() { return true; }
    void createBackendObject()
    {
        if (m_backendObject)
            return;

        P_Q(VideoGraphicsObject);
        m_backendObject = Factory::createVideoGraphicsObject(q);
        if (m_backendObject) {
            QObject::connect(m_backendObject, SIGNAL(frameReady()),
                             q, SLOT(frameReady()),
                             Qt::QueuedConnection);
            QObject::connect(m_backendObject, SIGNAL(reset()),
                             q, SLOT(reset()),
                             Qt::QueuedConnection);
            // Frameready triggers an update/paint, within paint we negotiate.
            QObject::connect(m_backendObject, SIGNAL(needFormat()),
                             q, SLOT(_p_negotiateFormat()),
                             Qt::DirectConnection);
            // FIXME: directconnection ...
            // this is actually super wrong, as for example with VLC we get a foreign thread
            // cb, so for the sake of not having to make this entire thing threadsafe, the backend
            // needs to sync their cb with us
        }
    }
};

void VideoGraphicsObjectPrivate::updateBoundingRect()
{
    P_Q(VideoGraphicsObject);
    emit q->prepareGeometryChange();

    // keep aspect
    QSizeF scaledFrameSize = frameSize;
    scaledFrameSize.scale(geometry.size(), Qt::KeepAspectRatio);

    boundingRect = QRectF(0, 0, scaledFrameSize.width(), scaledFrameSize.height());
    boundingRect.moveCenter(geometry.center());
}

/**
 * @brief VideoGraphicsObjectPrivate::_p_negotiateFormat
 * Super spooky function. There are two ways formats can be known to the VGO:
 *  * Previously detected by a (QML) Spy
 *  * This function is called by paint() allowing us to introspect capabilities.
 *
 * Once the supported painters & formats were detected the actual negotiation starts.
 * Currently this function always returns a painter, or it goes down in flames.
 *
 * @return Painter to use.
 */
void VideoGraphicsObjectPrivate::_p_negotiateFormat()
{
    QMap<GraphicsPainterType,QList<VideoFrame::Format> > formats;
    if (spyFormats.empty())
        formats = VideoGraphicsPainterMetaFactory::detectTypes();
    else
        formats = spyFormats;

    QList<VideoFrame::Format> formatList;
    QMap<GraphicsPainterType,QList<VideoFrame::Format> >::const_iterator it = formats.constBegin();
    while (it != formats.constEnd()) {
        foreach (VideoFrame::Format format, it.value()) {
            if (!formatList.contains(format))
                formatList.append(format);
        }
        ++it;
    }

    QList<VideoFrame::Format> choices;
    pBACKEND_GET1(QList<VideoFrame::Format>, choices, "offering", QList<VideoFrame::Format>, formatList);
    PrivatePainterFactory factory;
    graphicsPainter = factory.factorize(choices, formats);
    pINTERFACE_CALL(choose(factory.chosenFormat));
}

// --------------------------------- PUBLIC --------------------------------- //

VideoGraphicsObject::VideoGraphicsObject(QGraphicsItem *parent) :
    QGraphicsObject(parent),
    MediaNode(*new VideoGraphicsObjectPrivate)
{
    setFlag(ItemHasNoContents, false);

    P_D(VideoGraphicsObject);
    d->createBackendObject();
}

VideoGraphicsObject::~VideoGraphicsObject()
{
}

QRectF VideoGraphicsObject::boundingRect() const
{
    P_D(const VideoGraphicsObject);
    return d->boundingRect;
}

void VideoGraphicsObject::paint(QPainter *painter,
                                const QStyleOptionGraphicsItem *option,
                                QWidget *widget)
{
    P_D(VideoGraphicsObject);

    INTERFACE_CALL(lock());

    if (!d->graphicsPainter)
        d->_p_negotiateFormat();

#ifdef __GNUC__
#warning what if no painter could be created?
#endif
    Q_ASSERT(d->graphicsPainter);

    const VideoFrame *frame = INTERFACE_CALL(frame());

    if (frame->format != VideoFrame::Format_Invalid && !d->gotSize) {
        // TODO: do scaling ourselfs?
        d->gotSize = true;
        d->frameSize = QSize(frame->width, frame->height);
        d->updateBoundingRect();
    }

    if (frame->format == VideoFrame::Format_Invalid || !d->paintedOnce) {
        painter->fillRect(d->boundingRect, Qt::black);
        if (!d->paintedOnce) // Must not ever block here!
            QMetaObject::invokeMethod(this, "gotPaint", Qt::QueuedConnection);
        d->paintedOnce = true;
    } else {
        Q_ASSERT(d->graphicsPainter);
        d->graphicsPainter->setFrame(frame);
        if (!d->graphicsPainter->inited())
            d->graphicsPainter->init();
        d->graphicsPainter->paint(painter, d->boundingRect);
    }

    INTERFACE_CALL(unlock());
}

bool VideoGraphicsObject::canNegotiate() const
{
    P_D(const VideoGraphicsObject);
    return d->paintedOnce || !d->spyFormats.empty();
}

void VideoGraphicsObject::setGeometry(const QRectF &newGeometry)
{
    P_D(VideoGraphicsObject);
    d->geometry = newGeometry;
    d->updateBoundingRect();
}

void VideoGraphicsObject::frameReady()
{
    P_D(const VideoGraphicsObject);
    update(d->boundingRect);
}

void VideoGraphicsObject::reset()
{
    P_D(VideoGraphicsObject);
    // Do not reset the spyFormats as they will not change.
    d->paintedOnce = false;
    d->gotSize = false;

    // The painter is reset because the backend may choose another format for
    // another file (better conversion for some codec etc.)
    if (d->graphicsPainter) {
        delete d->graphicsPainter;
        d->graphicsPainter = 0;
    }
}

void VideoGraphicsObject::setSpyFormats(QMap<GraphicsPainterType,QList<VideoFrame::Format> > formats)
{
    P_D(VideoGraphicsObject);
    d->spyFormats = formats;
    // TODO: fake paint signal to ensure play() is issued?
}

} // namespace Phonon

#include "moc_videographicsobject.cpp"
