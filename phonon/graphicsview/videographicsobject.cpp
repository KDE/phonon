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

#define PHONON_INTERFACENAME VideoGraphicsObjectInterface

namespace Phonon {

// --------------------------------- Painter --------------------------------- //
class QPainterPainter : public AbstractVideoGraphicsPainter
{
public:
    QPainterPainter() {}
    virtual ~QPainterPainter() {}

    void init() {}
    void paint(QPainter *painter, QRectF target, const VideoFrame *frame)
    {
        painter->drawImage(target, frame->qImage());
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
        graphicsPainter(0)
    {}

    virtual ~VideoGraphicsObjectPrivate()
    {
        if (graphicsPainter)
            delete graphicsPainter;
    }

    virtual QObject *qObject() { return q_func(); }

    AbstractVideoGraphicsPainter *createPainter();
    void paintGl(QPainter *painter, QRectF rect, VideoFrame *frame);

    void setTargetRect();

    QRectF geometry;
    QRectF boundingRect;
    QSize frameSize;

    AbstractVideoGraphicsPainter *graphicsPainter;

protected:
    bool aboutToDeleteBackendObject() {}
    void createBackendObject()
    {
        if (m_backendObject)
            return;

        Q_Q(VideoGraphicsObject);
        m_backendObject = Factory::createVideoGraphicsObject(q);
        if (m_backendObject) {
            pINTERFACE_CALL(setVideoGraphicsObject(q));
            QObject::connect(m_backendObject, SIGNAL(frameReady()),
                             q, SLOT(frameReady()),
                             Qt::QueuedConnection);
        }
    }
};

VideoGraphicsObject::VideoGraphicsObject(QGraphicsItem *parent) :
    QGraphicsObject(parent),
    MediaNode(*new VideoGraphicsObjectPrivate)
{
    setFlag(ItemHasNoContents, false);

    K_D(VideoGraphicsObject);
    d->createBackendObject();
}

VideoGraphicsObject::~VideoGraphicsObject()
{
}

QRectF VideoGraphicsObject::boundingRect() const
{
    K_D(const VideoGraphicsObject);
    return d->boundingRect;
}

AbstractVideoGraphicsPainter *VideoGraphicsObjectPrivate::createPainter()
{
    AbstractVideoGraphicsPainter *painter = 0;
    if(QGLContext *glContext = const_cast<QGLContext *>(QGLContext::currentContext())) {
        glContext->makeCurrent();

        GlPainter *glPainter = 0;
        const QByteArray paintEnv(qgetenv("PHONON_PAINT")); // Used to override
        const QByteArray glExtensions(reinterpret_cast<const char *>(glGetString(GL_EXTENSIONS)));
        if (QGLShaderProgram::hasOpenGLShaderPrograms(glContext)
                && glExtensions.contains("ARB_shader_objects")
                && (paintEnv == QByteArray("glsl") || paintEnv == QByteArray(""))) {
            // Use GLSL.
            glPainter = new GlslPainter;
        } else if (glExtensions.contains("ARB_fragment_program")
                   && (paintEnv == QByteArray("arb") || paintEnv == QByteArray(""))) {
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

    painter->init();
    return painter;
}

void VideoGraphicsObject::paint(QPainter *painter,
                                const QStyleOptionGraphicsItem *option,
                                QWidget *widget)
{
    K_D(VideoGraphicsObject);

    static bool paintedOnce = false;
    static bool gotSize = false;

    INTERFACE_CALL(lock());

    const VideoFrame *frame = INTERFACE_CALL(frame());

    // NOTE: it would be most useful if we had a signal to notify about dimension changes...
    // NOTE: it would be even better if a frame contained a QRectF
    if (frame->format != VideoFrame::Format_Invalid &&
            !frame->qImage().isNull() &&
            !gotSize) {
        // TODO: do scaling ourselfs?
        gotSize = true;
        d->frameSize = QSize(frame->width, frame->height);
        d->setTargetRect();
    }

    if (frame->format == VideoFrame::Format_Invalid && !paintedOnce) {
        painter->fillRect(d->boundingRect, Qt::black);
    } else {
        if (!d->graphicsPainter)
            d->graphicsPainter = d->createPainter();
        d->graphicsPainter->paint(painter, d->boundingRect, frame);
    }

    INTERFACE_CALL(unlock());

    paintedOnce = true;
}

void VideoGraphicsObjectPrivate::paintGl(QPainter *painter, QRectF target, VideoFrame *frame)
{
    Q_Q(VideoGraphicsObject);
    QGLContext *ctx = const_cast<QGLContext *>(QGLContext::currentContext());
    GLuint texture = ctx->bindTexture(frame->qImage(),GL_TEXTURE_2D, QGLContext::NoBindOption);
    ctx->drawTexture(target, texture);
}

void VideoGraphicsObject::setGeometry(const QRectF &newGeometry)
{
    K_D(VideoGraphicsObject);
    d->geometry = newGeometry;
    d->setTargetRect();
}

void VideoGraphicsObjectPrivate::setTargetRect()
{
    Q_Q(VideoGraphicsObject);
    emit q->prepareGeometryChange();

    // keep aspect
    QSizeF frameSize = frameSize;
    frameSize.scale(geometry.size(), Qt::KeepAspectRatio);

    boundingRect = QRectF(0, 0, frameSize.width(), frameSize.height());
    boundingRect.moveCenter(geometry.center());
}

void VideoGraphicsObject::frameReady()
{
    K_D(const VideoGraphicsObject);
    update(d->boundingRect);
}

} // namespace Phonon
