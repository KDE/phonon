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

#ifndef PHONON_GLSLPAINTER_H
#define PHONON_GLSLPAINTER_H

#include <QtCore/QTime>

#include "glpainter.h"

class QGLShaderProgram;

namespace Phonon {

class GlslPainter : public GlPainter
{
public:
    GlslPainter();
    virtual ~GlslPainter();

    virtual QList<VideoFrame::Format> supportedFormats() const;

    void init();
    void paint(QPainter *painter, QRectF target);

private:
    void calculateFPS();
    void addFPSOverlay();

    struct FPS {
        FPS()
            : value(0)
            , imagedValue(0)
            , frames(0)
            , img(32, 32, QImage::Format_ARGB32)
        {
        }

        qreal value;
        qreal imagedValue;
        quint64 frames;
        QTime lastTime;
        QImage img;
    };

    struct FPS m_fps;
    QGLShaderProgram *m_program;
};

} // namespace Phonon

#endif // PHONON_GLSLPAINTER_H
