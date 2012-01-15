/*  This file is part of the KDE project
    Copyright (C) 2006 Matthias Kretz <kretz@kde.org>

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

#ifndef PHONON_EXPERIMENTAL_VIDEOFRAME_H
#define PHONON_EXPERIMENTAL_VIDEOFRAME_H

#include "export.h"
#include "videodataoutput.h"
#include <QtCore/QByteRef>

namespace Phonon
{
namespace Experimental
{
    /**
     * \brief A single video frame.
     *
     * This simple class contains the data of a frame and metadata describing
     * how to interpret the data.
     *
     * \author Matthias Kretz <kretz@kde.org>
     */
    struct PHONONEXPERIMENTAL_EXPORT VideoFrame
    {
        enum Format {
            Format_YUV422,
            Format_YUV420,
            Format_RGBA8
            //Format_RGBA8_Premultiplied,
            //Format_RGBA16F,
            //Format_RGBA32F,
            //Colorspace_XVYCC
        };
        //Qt::HANDLE handle() const;

        QByteArray data;
        /**
         * The width of the video frame in pixels.
         */
        int width;
        /**
         * The height of the video frame in pixels.
         */
        int height;
        /**
         * Colorspace of the frame
         */
        Format colorspace;
    };
} // namespace Experimental
} // namespace Phonon

// vim: sw=4 ts=4 tw=80
#endif // PHONON_EXPERIMENTAL_VIDEOFRAME_H
