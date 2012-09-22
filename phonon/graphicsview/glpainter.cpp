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

#include "glpainter.h"

namespace Phonon {

GlPainter::GlPainter() :
    m_context(0)
  , m_texturesInited(false)
{
}

GlPainter::~GlPainter()
{
#ifdef __GNUC__
#warning context may be long gone, leading to crashery
#endif
//    if (m_context) {
//        m_context->makeCurrent();
//        glDeleteTextures(m_textureCount, m_textureIds);
//    }
}

void GlPainter::setContext(QGLContext *context)
{
    m_context = context;
}

void GlPainter::initRgb32()
{
    Q_ASSERT(m_frame->planeCount == 1);
    m_textureCount = m_frame->planeCount;

    m_texDescriptor.target = GL_TEXTURE_2D;
    m_texDescriptor.internalFormat = GL_RGBA;
    m_texDescriptor.format = GL_RGBA;
    m_texDescriptor.type = GL_UNSIGNED_BYTE;
}

void GlPainter::initYv12()
{
    Q_ASSERT(m_frame->planeCount == 3);
    m_textureCount = m_frame->planeCount;

    m_texDescriptor.target = GL_TEXTURE_2D;
    m_texDescriptor.internalFormat = GL_LUMINANCE;
    m_texDescriptor.format = GL_LUMINANCE;
    m_texDescriptor.type = GL_UNSIGNED_BYTE;
}

void GlPainter::initColorMatrix()
{
    m_colorMatrix = QMatrix4x4(1.0, 0.0, 0.0, 0.0,
                               0.0, 1.0, 0.0, 0.0,
                               0.0, 0.0, 1.0, 0.0,
                               0.0, 0.0, 0.0, 1.0);
    // If the fame has YUV format, apply color correction:
    switch (m_frame->format) {
    case VideoFrame::Format_YV12:
        QMatrix4x4 colorSpaceMatrix;
        if (m_frame->height > 576)
            colorSpaceMatrix =
                    QMatrix4x4(
                        1.164383561643836,  0.0000,             1.792741071428571, -0.972945075016308,
                        1.164383561643836, -0.21324861427373,  -0.532909328559444,  0.301482665475862,
                        1.164383561643836,  2.112401785714286,  0.0000,            -1.133402217873451,
                        0.0000,             0.0000,             0.0000,             1.0000           );
        else
            colorSpaceMatrix =
                    QMatrix4x4(
                        1.164383561643836,  0.0000,             1.596026785714286, -0.874202217873451,
                        1.164383561643836, -0.391762290094914, -0.812967647237771,  0.531667823499146,
                        1.164383561643836,  2.017232142857142,  0.0000,            -1.085630789302022,
                        0.0000,             0.0000,             0.0000,             1.0000           );
        m_colorMatrix = m_colorMatrix * colorSpaceMatrix;
        break;
    }
}

void GlPainter::initTextures()
{
    if (!m_texturesInited) {
        for (unsigned i = 0; i < m_frame->planeCount; ++i) {
            glBindTexture(m_texDescriptor.target, m_textureIds[i]);
            glTexImage2D(m_texDescriptor.target,
                         0,
                         m_texDescriptor.internalFormat,
                         m_frame->visiblePitch[i],
                         m_frame->visibleLines[i],
                         0,
                         m_texDescriptor.format,
                         m_texDescriptor.type,
                         0);
            // Scale appropriately so we can change to target geometry without
            // much hassle.
            glTexParameterf(m_texDescriptor.target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameterf(m_texDescriptor.target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameterf(m_texDescriptor.target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameterf(m_texDescriptor.target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameterf(m_texDescriptor.target, GL_TEXTURE_PRIORITY, 1.0);
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            m_texturesInited = true;
        }
    }
    for (unsigned i = 0; i < m_frame->planeCount; ++i) {
        glBindTexture(m_texDescriptor.target, m_textureIds[i]);
        glPixelStorei(GL_UNPACK_ROW_LENGTH, m_frame->pitch[i]);
        glTexSubImage2D(m_texDescriptor.target, 0,
                        0, 0,
                        m_frame->visiblePitch[i],
                        m_frame->visibleLines[i],
                        m_texDescriptor.format,
                        m_texDescriptor.type,
                        m_frame->plane[i].data());
        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0); // reset to default
    }
}

} // namespace Phonon

