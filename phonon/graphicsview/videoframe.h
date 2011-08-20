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

#ifndef PHONON_VIDEOFRAME_H
#define PHONON_VIDEOFRAME_H

#include <QtGui/QImage>

namespace Phonon {

struct VideoFrame {
    enum Format {
        // TODO: really reference qimg formats?
        Format_Invalid = QImage::Format_Invalid,
        Format_RGB888 = QImage::Format_RGB888,
        Format_RGB32 = QImage::Format_RGB32,
        Format_YCbCr420 = 0x10000,
        Format_YV12 = Format_YCbCr420,
        Format_YCbCr422 = 0x10001,
        Format_YUY2 = Format_YCbCr422
    };

    int width;
    int height;

    Format format;

    QByteArray plane[3];
    int planeCount;

    inline QImage qImage() const
    {
        // QImage can only handle packed formats.
        if (planeCount != 1)
            return QImage();

        switch(format) {
        case Format_RGB32:
            return QImage(reinterpret_cast<const uchar *>(plane[0].constData()),
                          width, height, QImage::Format_RGB32);
        default:
            return QImage();
        }
    }
};

} // namespace Phonon

#endif // PHONON_VIDEOFRAME_H
