/*
    Copyright (C) 2011-2012 Harald Sitter <sitter@kde.org>
    Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies)

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

#include "glslpainter.h"

#include <QtOpenGL/QGLShaderProgram>

#include "videoframe.h"

namespace Phonon {

void GlslPainter::calculateFPS()
{
    if (m_fps.lastTime.isNull())
        m_fps.lastTime = QTime::currentTime();
    QTime time = QTime::currentTime();

    int delta = m_fps.lastTime.msecsTo(time);
    if (delta > 2000) {
        m_fps.value = 1000.0 * m_fps.frames / qreal(delta);
        if (m_fps.value < 20.0)
            qWarning() << "Drawing less than 20 frames per second!";
        m_fps.lastTime = time;
        m_fps.frames = 0;
    }

    ++m_fps.frames;
}

void GlslPainter::addFPSOverlay()
{
    if (m_fps.value != m_fps.imagedValue) {
        // Update image
        m_fps.img.fill(Qt::blue);
        QPainter painter(&(m_fps.img));
        painter.setPen(QColor(Qt::white));
        painter.drawText(0, 16, QString::number((int)m_fps.value));
        painter.end();
    }

    GLuint id = m_context->bindTexture(m_fps.img);
    m_context->drawTexture(QPointF(0, 0), id);
    m_context->deleteTexture(id);

    m_fps.imagedValue = m_fps.value;
}

static const char *s_phonon_rgb32Shader =
"uniform sampler2D textureSampler;\n"
"varying highp vec2 textureCoord;\n"
"void main(void)\n"
"{\n"
"    gl_FragColor = vec4(texture2D(textureSampler, textureCoord.st).bgr, 1.0);\n"
"}\n";

static const char *s_phonon_yv12Shader =
"uniform sampler2D texY;\n"
"uniform sampler2D texU;\n"
"uniform sampler2D texV;\n"
"uniform mediump mat4 colorMatrix;\n"
"varying highp vec2 textureCoord;\n"
"uniform lowp float opacity;"
"void main(void)\n"
"{\n"
"    highp vec4 color = vec4(\n"
"           texture2D(texY, textureCoord.st).r,\n"
"           texture2D(texV, textureCoord.st).r,\n" // !!!! mind the swp
"           texture2D(texU, textureCoord.st).r,\n"
"           1.0);\n"
"    gl_FragColor = colorMatrix * color * opacity;\n"
"}\n";

GlslPainter::GlslPainter() :
    m_program(0)
{}

GlslPainter::~GlslPainter()
{
#ifdef __GNUC__
#warning context may be long gone, leading to crashery
#endif
    if (m_program) {
        m_program->removeAllShaders();
        m_program->deleteLater();
    }
}

QList<VideoFrame::Format> GlslPainter::supportedFormats() const
{
    QList<VideoFrame::Format> formats;

    QGLContext *glContext = const_cast<QGLContext *>(QGLContext::currentContext());
    if (glContext) {
        glContext->makeCurrent();

        const QByteArray glExtensions(reinterpret_cast<const char *>(glGetString(GL_EXTENSIONS)));
        if (QGLShaderProgram::hasOpenGLShaderPrograms(glContext)
                && glExtensions.contains("ARB_shader_objects")) {
            // We are usable.
            return formats << VideoFrame::Format_YV12
                           << VideoFrame::Format_RGB32;
        }
    }

    return formats;
}

void GlslPainter::init()
{
    m_context = const_cast<QGLContext *>(QGLContext::currentContext());
    Q_ASSERT(m_context);
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

    const char *program = 0;
    switch (m_frame->format) {
    case VideoFrame::Format_RGB32://////////////////////////////////////// RGB32
        initRgb32();
        program = s_phonon_rgb32Shader;
        break;
    case VideoFrame::Format_YV12: ///////////////////////////////////////// YV12
        initYv12();
        program = s_phonon_yv12Shader;
        break;
    default: /////////////////////////////////////////////////////////// Default
        qDebug() << "format: " << m_frame->format;
        Q_ASSERT(false);
    }
    Q_ASSERT(program);
    initColorMatrix();

    if (!m_program->addShaderFromSourceCode(QGLShader::Vertex, vertexProgram))
        qFatal("couldnt add vertex shader");
    else if (!m_program->addShaderFromSourceCode(QGLShader::Fragment, program))
        qFatal("couldnt add fragment shader");
    else if (!m_program->link())
        qFatal("couldnt link shader");

    glGenTextures(m_textureCount, m_textureIds);

    m_inited = true;
}

void GlslPainter::paint(QPainter *painter, QRectF target)
{
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
    initTextures();
    //////////////////////////////////////////////////////////////

    // As seen on the telly
#ifdef __GNUC__
#warning DUPLICATED CODE
#endif

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

    if (m_textureCount == 3) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_textureIds[0]);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_textureIds[1]);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, m_textureIds[2]);
        glActiveTexture(GL_TEXTURE0);

        m_program->setUniformValue("texY", 0);
        m_program->setUniformValue("texU", 1);
        m_program->setUniformValue("texV", 2);
    } else {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_textureIds[0]);

        m_program->setUniformValue("texRgb", 0);
    }
    m_program->setUniformValue("colorMatrix", m_colorMatrix);
    m_program->setUniformValue("opacity", GLfloat(painter->opacity()));

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    m_program->release();
    painter->endNativePainting();

    calculateFPS();
    addFPSOverlay();
}

} // namespace Phonon
