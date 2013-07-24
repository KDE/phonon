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

#ifndef PHONON_GLPAINTER_H
#define PHONON_GLPAINTER_H

#include <QtGui/QMatrix4x4>
// Not forward included, because we need platform specific GL typedefs for structs.
#include <QtOpenGL/QGLContext>

#include "abstractvideographicspainter.h"
#include "videoframe.h"

namespace Phonon {

class GlPainter : public AbstractVideoGraphicsPainter
{
    typedef struct {
        GLenum target;
        GLint internalFormat;
//        GLint border;
        GLenum format;
        GLenum type;
    } GlTextureDescriptor;

    typedef struct {
        GLsizei width;
        GLsizei height;
    } GlTextureSize;

public:
    /** Destructor. */
    virtual ~GlPainter();

    /** \param context the QGLContext to use for this painter. */
    void setContext(QGLContext *context);

    /** Initialize for an RGB32/RGBA frame. */
    void initRgb32();

    /** Initialize for an YV12 frame. */
    void initYv12();

    /** Initialize the color matrix to be used for YUV->RGB color conversion. */
    void initColorMatrix();

    /**
     * Initialize all textures (amount of texture is dependent on what count was
     * decided in the specific init functions.
     */
    void initTextures();

protected:
    GlPainter();

    QGLContext *m_context;
    int m_textureCount;
    GLuint m_textureIds[3];

    GlTextureDescriptor m_texDescriptor;
    GlTextureSize m_texSize[3];

    QMatrix4x4 m_colorMatrix;

    bool m_texturesInited;
};

} // namespace Phonon

#endif // PHONON_GLPAINTER_H
