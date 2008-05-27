/*  This file is part of the KDE project
    Copyright (C) 2008 Matthias Kretz <kretz@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License version 2 as published by the Free Software Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.

*/

#ifndef PHONON_VIDEODATAOUTPUT2_H
#define PHONON_VIDEODATAOUTPUT2_H

#include "export.h"
#include <QtCore/QObject>
#include "abstractvideodataoutput.h"

namespace Phonon
{
namespace Experimental
{

class VideoDataOutput2Private;

/**
 * \short This class gives you the video data.
 *
 * This class implements a special AbstractVideoOutput that gives your
 * application the video data.
 *
 * You can also use the video data for further processing (e.g. encoding and
 * saving to a file).
 *
 * \author Matthias Kretz <kretz@kde.org>
 */
class PHONONEXPERIMENTAL_EXPORT VideoDataOutput2 : public QObject, public AbstractVideoDataOutput
{
    Q_OBJECT
    K_DECLARE_PRIVATE(VideoDataOutput2)
    PHONON_HEIR(VideoDataOutput2)

    protected:
        virtual void frameReady(const VideoFrame2 &);

        virtual void endOfMedia();

    Q_SIGNALS:
        /**
         * The signal is emitted whenever a frame should be displayed.
         * nowStamp is the current time, outStamp tells the users
         * what time the frame should be displayed with.
         *
         * The relevant frames should be fetched and displayed using frameForTime
         * method.
         */
        void frameReadySignal(const VideoFrame2 &);

        /**
         * This signal is emitted after the last frameReady signal of a
         * media is emitted.
         */
        void endOfMediaSignal();
};

} // namespace Experimental
} //namespace Phonon

#endif // PHONON_VIDEODATAOUTPUT2_H
