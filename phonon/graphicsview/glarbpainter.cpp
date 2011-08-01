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

#include "glarbpainter.h"

#include "videoframe.h"

namespace Phonon {

GlArbPainter::GlArbPainter() :
    m_useMultitexture(false),
    m_maxTextureUnits(0)
{
}

GlArbPainter::~GlArbPainter()
{
    if (m_context) {
        m_context->makeCurrent();
        glDeleteProgramsARB(1, &programId);
    }
}

void GlArbPainter::init()
{
    Q_ASSERT(m_context);
    m_context->makeCurrent();

    glProgramStringARB = (_glProgramStringARB) m_context->getProcAddress(
                QLatin1String("glProgramStringARB"));
    glBindProgramARB = (_glBindProgramARB) m_context->getProcAddress(
                QLatin1String("glBindProgramARB"));
    glDeleteProgramsARB = (_glDeleteProgramsARB) m_context->getProcAddress(
                QLatin1String("glDeleteProgramsARB"));
    glGenProgramsARB = (_glGenProgramsARB) m_context->getProcAddress(
                QLatin1String("glGenProgramsARB"));

    glActiveTextureARB = (_glActiveTextureARB) m_context->getProcAddress(
                QLatin1String("glActiveTextureARB"));
    glMultiTexCoord2fARB = (_glMultiTexCoord2fARB) m_context->getProcAddress(
                QLatin1String("glMultiTexCoord2fARB"));
    if (glActiveTextureARB && glMultiTexCoord2fARB)
        glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, &m_maxTextureUnits);

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

void GlArbPainter::paint(QPainter *painter, QRectF target, const VideoFrame *frame)
{
    Q_ASSERT(m_context);
    m_context->makeCurrent();

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
                 frame->plane[0].constData());
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

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisable(GL_FRAGMENT_PROGRAM_ARB);

    painter->endNativePainting();
}

} // namespace Phonon
