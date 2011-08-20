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

#ifndef PHONON_GLARBPAINTER_H
#define PHONON_GLARBPAINTER_H

#include <QtGui/QMatrix4x4>

#include "glpainter.h"

namespace Phonon {

class GlArbPainter : public GlPainter
{
public:
    GlArbPainter();
    virtual ~GlArbPainter();

    void init();
    void paint(QPainter *painter, QRectF target);

private:
    GLuint programId;

    // Function pointers filled using getProcAddress()
    typedef void (APIENTRY *_glProgramStringARB) (GLenum, GLenum, GLsizei, const GLbyte *);
    typedef void (APIENTRY *_glBindProgramARB) (GLenum, GLuint);
    typedef void (APIENTRY *_glDeleteProgramsARB) (GLsizei, const GLuint *);
    typedef void (APIENTRY *_glGenProgramsARB) (GLsizei, GLuint *);
    typedef void (APIENTRY *_glProgramLocalParameter4fARB) (
            GLenum, GLuint, GLfloat, GLfloat, GLfloat, GLfloat);

    _glProgramStringARB glProgramStringARB;
    _glBindProgramARB glBindProgramARB;
    _glDeleteProgramsARB glDeleteProgramsARB;
    _glGenProgramsARB glGenProgramsARB;
    _glProgramLocalParameter4fARB glProgramLocalParameter4fARB;

    // Multitexture
    typedef void (APIENTRY *_glActiveTextureARB) (GLenum);
    typedef void (APIENTRY *_glMultiTexCoord2fARB) (GLenum, GLfloat, GLfloat);

    _glActiveTextureARB glActiveTextureARB;
    _glMultiTexCoord2fARB glMultiTexCoord2fARB;

    bool m_useMultitexture;
    int m_maxTextureUnits;
};

} // namespace Phonon

#endif // PHONON_GLARBPAINTER_H
