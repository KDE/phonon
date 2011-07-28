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

#include <QtOpenGL>

#include "factory_p.h"
#include "medianode_p.h"
#include "phonondefs_p.h"

#define PHONON_INTERFACENAME VideoGraphicsObjectInterface

namespace Phonon {

class VideoGraphicsPainter
{
public:
    virtual void init() = 0;
    virtual void paint(QPainter *painter, QRectF target, VideoFrame *frame) = 0;

protected:
    VideoGraphicsPainter() {}
    virtual ~VideoGraphicsPainter() {}
};

class GlPainter : public VideoGraphicsPainter
{
protected:
    GlPainter() :
        m_context(0)
    {}
    virtual ~GlPainter() {}

    QGLContext *m_context;
    int m_textureCount;
    GLuint m_textureIds[3];

};

class GlslPainter : public GlPainter
{
public:
    GlslPainter() :
        m_program(0)
    {}

    ~GlslPainter()
    {
        if (m_program)
            m_program->deleteLater();
    }

    void init()
    {
        if (m_context) {
    #warning factor into own function
            m_context = const_cast<QGLContext *>(QGLContext::currentContext());
            m_context->makeCurrent();
            return;
        }

        m_context = const_cast<QGLContext *>(QGLContext::currentContext());
        m_context->makeCurrent();

        if (!m_program)
            m_program = new QGLShaderProgram(m_context);

        const char *vertexProgram =
                "attribute highp vec4 targetVertex;\n"
                "attribute highp vec2 textureCoordinates;\n"
                "uniform highp mat4 positionMatrix;\n"
                "varying highp vec2 textureCoord;\n"
                "void main(void)\n"
                "{\n"
                "    gl_Position = positionMatrix * targetVertex;\n"
                "    textureCoord = textureCoordinates;\n"
                "}\n";

        const char *program =
                "uniform sampler2D textureSampler;\n"
                "varying highp vec2 textureCoord;\n"
                "void main(void)\n"
                "{\n"
                "    gl_FragColor = vec4(texture2D(textureSampler, textureCoord.st).bgr, 1.0);\n"
                "}\n";

        m_textureCount = 1;

        if (!m_program->addShaderFromSourceCode(QGLShader::Vertex, vertexProgram))
            qFatal("couldnt add vertex shader");
        else if (!m_program->addShaderFromSourceCode(QGLShader::Fragment, program))
            qFatal("couldnt add fragment shader");
        else if (!m_program->link())
            qFatal("couldnt link shader");

        glGenTextures(m_textureCount, m_textureIds);
    }

    void paint(QPainter *painter, QRectF target, VideoFrame *frame)
    {
        init();

        //////////////////////////////////////////////////////////////

        // Need to reenable those after native painting has begun, otherwise we might
        // not be able to paint anything.
        bool stencilTestEnabled = glIsEnabled(GL_STENCIL_TEST);
        bool scissorTestEnabled = glIsEnabled(GL_SCISSOR_TEST);

        painter->beginNativePainting();

        if (stencilTestEnabled)
            glEnable(GL_STENCIL_TEST);
        if (scissorTestEnabled)
            glEnable(GL_SCISSOR_TEST);

        //////////////////////////////////////////////////////////////
    #warning factor into own function

    #warning multitexture support for yuv
        glBindTexture(GL_TEXTURE_2D, m_textureIds[0]);
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_RGBA,
                     frame->width, frame->height,
                     0,
                     GL_RGBA,
                     GL_UNSIGNED_BYTE,
                 #warning data needs changing for sane access!!
                     frame->data);
        // Scale appropriately so we can change to target geometry without
        // much hassle.
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        //////////////////////////////////////////////////////////////

        // As seen on the telly
    #warning DUPLICATED CODE

        const float textureCoordinates[] = {
            0, 1, // bottom left
            1, 1, // bottom right
            0, 0, // top left
            1, 0, // top right
        };

        const GLfloat targetVertex[] =
        {
            GLfloat(target.left()), GLfloat(target.bottom()),
            GLfloat(target.right()), GLfloat(target.bottom()),
            GLfloat(target.left()) , GLfloat(target.top()),
            GLfloat(target.right()), GLfloat(target.top())
        };
        //

        const int width = QGLContext::currentContext()->device()->width();
        const int height = QGLContext::currentContext()->device()->height();

        const QTransform transform = painter->deviceTransform();

        const GLfloat wfactor = 2.0 / width;
        const GLfloat hfactor = -2.0 / height;

        const GLfloat positionMatrix[4][4] = {
            {
                GLfloat(wfactor * transform.m11() - transform.m13()),
                GLfloat(hfactor * transform.m12() + transform.m13()),
                0.0,
                GLfloat(transform.m13())
            }, {
                GLfloat(wfactor * transform.m21() - transform.m23()),
                GLfloat(hfactor * transform.m22() + transform.m23()),
                0.0,
                GLfloat(transform.m23())
            }, {
                0.0,
                0.0,
                -1.0,
                0.0
            }, {
                GLfloat(wfactor * transform.dx() - transform.m33()),
                GLfloat(hfactor * transform.dy() + transform.m33()),
                0.0,
                GLfloat(transform.m33())
            }
        };

        m_program->bind();

        m_program->enableAttributeArray("targetVertex");
        m_program->enableAttributeArray("textureCoordinates");
        m_program->setAttributeArray("targetVertex", targetVertex, 2);
        m_program->setAttributeArray("textureCoordinates", textureCoordinates, 2);

        m_program->setUniformValue("positionMatrix", positionMatrix);

    #warning no idea how to do yuv with glsl :S
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_textureIds[0]);
        m_program->setUniformValue("texRgb", 0);

        glDrawArrays(GL_QUAD_STRIP, 0, 4);

        m_program->release();
        painter->endNativePainting();
    }

private:
    QGLShaderProgram *m_program;
};

class GlArbPainter : public GlPainter
{
public:
    GlArbPainter() {}
    ~GlArbPainter()
    {
        QGLContext *context = const_cast<QGLContext *>(QGLContext::currentContext());
        if (context) {
            context->makeCurrent();

            glDeleteTextures(m_textureCount, m_textureIds);
            glDeleteProgramsARB(1, &programId);
        }
    }

    void init()
    {
        if (m_context) {
    #warning factor into own function
            m_context = const_cast<QGLContext *>(QGLContext::currentContext());
            m_context->makeCurrent();
            return;
        }

        m_context = const_cast<QGLContext *>(QGLContext::currentContext());
        m_context->makeCurrent();

        glProgramStringARB = (_glProgramStringARB) m_context->getProcAddress(
                    QLatin1String("glProgramStringARB"));
        glBindProgramARB = (_glBindProgramARB) m_context->getProcAddress(
                    QLatin1String("glBindProgramARB"));
        glDeleteProgramsARB = (_glDeleteProgramsARB) m_context->getProcAddress(
                    QLatin1String("glDeleteProgramsARB"));
        glGenProgramsARB = (_glGenProgramsARB) m_context->getProcAddress(
                    QLatin1String("glGenProgramsARB"));

    #warning should be moved to macro or something
        const char *program =
                "!!ARBfp1.0\n"
                "TEMP rgb;\n"
                "TEX rgb.xyz, fragment.texcoord[0], texture[0], 2D;\n"
                "DP4 result.color.x, rgb.zyxw, { 1.0, 0.0, 0.0, 0.0 }  ;\n"
                "DP4 result.color.y, rgb.zyxw, { 0.0, 1.0, 0.0, 0.0 }  ;\n"
                "DP4 result.color.z, rgb.zyxw, { 0.0, 0.0, 1.0, 0.0 }  ;\n"
                "MOV rgb.w,                    { 0.0, 0.0, 0.0, 1.0 }.w;\n"
                "END";

    #warning > for yuv, needs fixing
        m_textureCount = 1;

        glGenProgramsARB(1, &programId);

        if (glGetError() != GL_NO_ERROR)
            qFatal("got GL error at program alloc");

        glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, programId);
        glProgramStringARB(GL_FRAGMENT_PROGRAM_ARB,
                           GL_PROGRAM_FORMAT_ASCII_ARB,
                           qstrlen(program),
                           reinterpret_cast<const GLvoid *>(program));

        if (glGetError() != GL_NO_ERROR)
            qFatal("got GL error at program making");

        glGenTextures(m_textureCount, m_textureIds);
    }

    void paint(QPainter *painter, QRectF target, VideoFrame *frame)
    {
        init();

        //////////////////////////////////////////////////////////////

        // Need to reenable those after native painting has begun, otherwise we might
        // not be able to paint anything.
        bool stencilTestEnabled = glIsEnabled(GL_STENCIL_TEST);
        bool scissorTestEnabled = glIsEnabled(GL_SCISSOR_TEST);

        painter->beginNativePainting();

        if (stencilTestEnabled)
            glEnable(GL_STENCIL_TEST);
        if (scissorTestEnabled)
            glEnable(GL_SCISSOR_TEST);

        //////////////////////////////////////////////////////////////
    #warning factor into own function

    #warning multitexture support for yuv
        glBindTexture(GL_TEXTURE_2D, m_textureIds[0]);
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_RGBA,
                     frame->width, frame->height,
                     0,
                     GL_RGBA,
                     GL_UNSIGNED_BYTE,
                 #warning data needs changing for sane access!!
                     frame->data);
        // Scale appropriately so we can change to target geometry without
        // much hassle.
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        //////////////////////////////////////////////////////////////

        // Target coordinates of the source rectangle, needs to be same aspect ratio,
        // scaling happens through magnifying/minifying filters.
        const GLfloat targetVertex[] = {
            GLfloat(target.left()) , GLfloat(target.bottom()),
            GLfloat(target.right()), GLfloat(target.bottom()),
            GLfloat(target.left()) , GLfloat(target.top()),
            GLfloat(target.right()), GLfloat(target.top())
        };

        // Normalized source rectangle, using anything >0 && <1 means clipping.
        const float textureCoordinates[] = {
            0, 1, // bottom left
            1, 1, // bottom right
            0, 0, // top left
            1, 0, // top right
        };

        glEnable(GL_FRAGMENT_PROGRAM_ARB);
        glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, programId);

    #warning for YUV we'll need to add the other 2 textures accordingly
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_textureIds[0]);

        glVertexPointer(2, GL_FLOAT, 0, targetVertex);
        glTexCoordPointer(2, GL_FLOAT, 0, textureCoordinates);

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        glDrawArrays(GL_QUAD_STRIP, 0, 4);

        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisable(GL_FRAGMENT_PROGRAM_ARB);

        painter->endNativePainting();
    }

private:
    GLuint programId;

    // Function pointers filled using getProcAddress()
    typedef void (APIENTRY *_glProgramStringARB) (GLenum, GLenum, GLsizei, const GLvoid *);
    typedef void (APIENTRY *_glBindProgramARB) (GLenum, GLuint);
    typedef void (APIENTRY *_glDeleteProgramsARB) (GLsizei, const GLuint *);
    typedef void (APIENTRY *_glGenProgramsARB) (GLsizei, GLuint *);

    _glProgramStringARB glProgramStringARB;
    _glBindProgramARB glBindProgramARB;
    _glDeleteProgramsARB glDeleteProgramsARB;
    _glGenProgramsARB glGenProgramsARB;
};

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
    {}

    virtual QObject *qObject() { return q_func(); }

    void paintGl(QPainter *painter, QRectF rect, VideoFrame *frame);

    QRectF geometry;
    QRectF boundingRect;
    QSize frameSize;

    VideoGraphicsPainter *graphicsPainter;

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

void VideoGraphicsObject::paint(QPainter *painter,
                                const QStyleOptionGraphicsItem *option,
                                QWidget *widget)
{
    K_D(VideoGraphicsObject);

    static bool paintedOnce = false;
    static bool gotSize = false;

    INTERFACE_CALL(lock());

    VideoFrame frame = *INTERFACE_CALL(frame());

    // NOTE: it would be most useful if we had a signal to notify about dimension changes...
    // NOTE: it would be even better if a frame contained a QRectF
    if (frame.format != VideoFrame::Format_Invalid &&
            !frame.qImage().isNull() &&
            !gotSize) {
        // TODO: do scaling ourselfs?
        gotSize = true;
        d->frameSize = QSize(frame.width, frame.height);
        setTargetRect();
    }

    if (frame.format == VideoFrame::Format_Invalid && !paintedOnce) {
        painter->fillRect(d->boundingRect, Qt::black);
    } else if (!frame.qImage().isNull()){
        QByteArray paintEnv = qgetenv("PHONON_PAINT");
        if (QGLContext::currentContext() && paintEnv == QByteArray("glsl")) {
            if (!d->graphicsPainter)
                d->graphicsPainter = new GlslPainter;
            d->graphicsPainter->paint(painter, d->boundingRect, &frame);
        } else if (QGLContext::currentContext() && paintEnv == QByteArray("glarb")) {
            if (!d->graphicsPainter)
                d->graphicsPainter = new GlArbPainter;
            d->graphicsPainter->paint(painter, d->boundingRect, &frame);
        } else if (QGLContext::currentContext() && paintEnv == QByteArray("gl")) {
            d->paintGl(painter, d->boundingRect, &frame);
        } else {
            painter->drawImage(d->boundingRect, frame.qImage());
        }
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
    setTargetRect();
}

void VideoGraphicsObject::setTargetRect()
{
    K_D(VideoGraphicsObject);

    emit prepareGeometryChange();

    // keep aspect
    QSizeF frameSize = d->frameSize;
    frameSize.scale(d->geometry.size(), Qt::KeepAspectRatio);

    d->boundingRect = QRectF(0, 0, frameSize.width(), frameSize.height());
    d->boundingRect.moveCenter(d->geometry.center());
}

void VideoGraphicsObject::frameReady()
{
    K_D(const VideoGraphicsObject);
    update(d->boundingRect);
}

} // namespace Phonon
