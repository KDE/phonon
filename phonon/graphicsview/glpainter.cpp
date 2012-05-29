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
{
}

GlPainter::~GlPainter()
{
#warning context may be long gone, leading to crashery
//    if (m_context) {
//        m_context->makeCurrent();
//        glDeleteTextures(m_textureCount, m_textureIds);
//    }
}

void GlPainter::setContext(QGLContext *context)
{
    m_context = context;
}

#define s(_d, p_n, p_d)  _d  * p_n / p_d
void GlPainter::initRgb32()
{
    Q_ASSERT(m_frame->planeCount == 1);
    m_textureCount = m_frame->planeCount;

    m_texDescriptor.target = GL_TEXTURE_2D;
    m_texDescriptor.internalFormat = GL_RGBA;
    m_texDescriptor.format = GL_RGBA;
    m_texDescriptor.type = GL_UNSIGNED_BYTE;

    m_texSize[0].width  = s(m_frame->width,  1, 1);
    m_texSize[0].height = s(m_frame->height, 1, 1);
}

void GlPainter::initYv12()
{
    Q_ASSERT(m_frame->planeCount == 3);
    m_textureCount = m_frame->planeCount;

    m_texDescriptor.target = GL_TEXTURE_2D;
    m_texDescriptor.internalFormat = GL_LUMINANCE;
    m_texDescriptor.format = GL_LUMINANCE;
    m_texDescriptor.type = GL_UNSIGNED_BYTE;

#warning there must be a better way!
    m_texSize[0].width  = s(m_frame->width,  1, 1);
    m_texSize[0].height = s(m_frame->height, 1, 1);
    m_texSize[1].width  = s(m_frame->width,  1, 2);
    m_texSize[1].height = s(m_frame->height, 1, 2);
    m_texSize[2].width  = s(m_frame->width,  1, 2);
    m_texSize[2].height = s(m_frame->height, 1, 2);
}
#undef s

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
            colorSpaceMatrix = QMatrix4x4(1.1640,  0.0000,  1.5701, -0.8612,
                                          1.1640, -0.1870, -0.4664,  0.2549,
                                          1.1640,  1.8556,  0.0000, -1.0045,
                                          0.0000,  0.0000,  0.0000,  1.0000);
        else
            colorSpaceMatrix = QMatrix4x4(1.1640,  0.0000,  1.4030, -0.7773,
                                          1.1640, -0.3440, -0.7140,  0.4580,
                                          1.1640,  1.7730,  0.0000, -0.9630,
                                          0.0000,  0.0000,  0.0000,  1.0000);
        m_colorMatrix = m_colorMatrix * colorSpaceMatrix;
        break;
    }
}

void GlPainter::initTextures()
{
    for (int i = 0; i < m_textureCount; ++i) {
        glBindTexture(GL_TEXTURE_2D, m_textureIds[i]);
        glTexImage2D(m_texDescriptor.target,
                     0,
                     m_texDescriptor.internalFormat,
                     m_texSize[i].width,
                     m_texSize[i].height,
                     0,
                     m_texDescriptor.format,
                     m_texDescriptor.type,
                     m_frame->plane[i].data());
        // Scale appropriately so we can change to target geometry without
        // much hassle.
        glTexParameterf(m_texDescriptor.target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(m_texDescriptor.target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(m_texDescriptor.target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(m_texDescriptor.target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameterf(m_texDescriptor.target, GL_TEXTURE_PRIORITY, 1.0);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    }
}

} // namespace Phonon

