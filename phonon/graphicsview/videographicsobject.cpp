/*
    Copyright (C) 2011 Harald Sitter <sitter@kde.org>

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

#include <QtGui/QPainter>
#warning
#include <QtOpenGL/QGLShaderProgram>

#include "abstractvideographicspainter.h"
#include "factory_p.h"
#include "glarbpainter.h"
#include "glslpainter.h"
#include "medianode_p.h"
#include "phonondefs_p.h"
#include "videoframe.h"
#include "videographicsobjectinterface.h"

#define PHONON_INTERFACENAME VideoGraphicsObjectInterface

namespace Phonon {

// --------------------------------- Painter --------------------------------- //
class QPainterPainter : public AbstractVideoGraphicsPainter
{
public:
    QPainterPainter() {}
    virtual ~QPainterPainter() {}

    void init() {}
    void paint(QPainter *painter, QRectF target)
    {
        // QImage can only handle packed formats.
        if (m_frame->planeCount != 1 || m_frame->format != VideoFrame::Format_RGB32) {
            painter->drawImage(target, QImage());
        } else {
            painter->drawImage(target,
                               QImage(reinterpret_cast<const uchar *>(m_frame->plane[0].constData()),
                                      m_frame->width, m_frame->height,
                                      QImage::Format_RGB32));
        }
    }
};

// --------------------------------- OBJECT --------------------------------- //

class VideoGraphicsObjectPrivate : public MediaNodePrivate
{
    Q_DECLARE_PUBLIC(VideoGraphicsObject)
public:
    VideoGraphicsObjectPrivate() :
        geometry(0, 0, 320, 240),
        boundingRect(0, 0, 0, 0),
        frameSize(0, 0),
        graphicsPainter(0),
        paintedOnce(false),
        gotSize(false)
    {}

    virtual ~VideoGraphicsObjectPrivate()
    {
        if (graphicsPainter)
            delete graphicsPainter;
    }

    virtual QObject *qObject() { return q_func(); }

    AbstractVideoGraphicsPainter *createPainter(const VideoFrame *frame);

    void updateBoundingRect();

    QRectF geometry;
    QRectF boundingRect;
    QSize frameSize;

    AbstractVideoGraphicsPainter *graphicsPainter;

    bool paintedOnce;
    bool gotSize;

protected:
    bool aboutToDeleteBackendObject() {}
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
        }
    }
};

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

AbstractVideoGraphicsPainter *VideoGraphicsObjectPrivate::createPainter(const VideoFrame *frame)
{
    AbstractVideoGraphicsPainter *painter = 0;

    // Used to override automatic painter selection.
    const QByteArray paintEnv(qgetenv("PHONON_PAINT"));
    QGLContext *glContext = const_cast<QGLContext *>(QGLContext::currentContext());

    if (paintEnv != QByteArray("qpainter") && glContext) {
        glContext->makeCurrent();

        GlPainter *glPainter = 0;
        const QByteArray glExtensions(reinterpret_cast<const char *>(glGetString(GL_EXTENSIONS)));
        if (QGLShaderProgram::hasOpenGLShaderPrograms(glContext)
                && glExtensions.contains("ARB_shader_objects")
                && (paintEnv == QByteArray("glsl") || paintEnv == QByteArray(""))) {
            // Use GLSL.
            glPainter = new GlslPainter;
        } else if (glExtensions.contains("ARB_fragment_program")
                   && (paintEnv == QByteArray("glarb") || paintEnv == QByteArray(""))) {
            // Use GLARB painter.
            glPainter = new GlArbPainter;
        }
#warning a qpainter glpaint would be good.

        glPainter->setContext(glContext);
        painter = glPainter;
    }

    if (!painter)
        // If all fails, just use QPainter's builtin functions.
        painter = new QPainterPainter;

    painter->setFrame(frame);
    painter->init();
    return painter;
}

void VideoGraphicsObject::paint(QPainter *painter,
                                const QStyleOptionGraphicsItem *option,
                                QWidget *widget)
{
    P_D(VideoGraphicsObject);

    INTERFACE_CALL(lock());

    const VideoFrame *frame = INTERFACE_CALL(frame());

    if (frame->format != VideoFrame::Format_Invalid && !d->gotSize) {
        // TODO: do scaling ourselfs?
        d->gotSize = true;
        d->frameSize = QSize(frame->width, frame->height);
        d->updateBoundingRect();
    }

    if (frame->format == VideoFrame::Format_Invalid || !d->paintedOnce) {
        painter->fillRect(d->boundingRect, Qt::black);
        d->paintedOnce = true;
    } else {
        if (!d->graphicsPainter)
            d->graphicsPainter = d->createPainter(frame);
        d->graphicsPainter->setFrame(frame);
        d->graphicsPainter->paint(painter, d->boundingRect);
    }

    INTERFACE_CALL(unlock());
}

void VideoGraphicsObject::setGeometry(const QRectF &newGeometry)
{
    P_D(VideoGraphicsObject);
    d->geometry = newGeometry;
    d->updateBoundingRect();
}

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

void VideoGraphicsObject::frameReady()
{
    P_D(const VideoGraphicsObject);
    update(d->boundingRect);
}

void VideoGraphicsObject::reset()
{
    P_D(VideoGraphicsObject);
    d->paintedOnce = false;
    d->gotSize = false;
    if (d->graphicsPainter) {
        delete d->graphicsPainter;
        d->graphicsPainter = 0;
    }
}

} // namespace Phonon
