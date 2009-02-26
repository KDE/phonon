/*  This file is part of the KDE project
    Copyright (C) 2005-2008 Matthias Kretz <kretz@kde.org>

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

#ifndef PHONON_X_ABSTRACTVIDEODATAOUTPUT_H
#define PHONON_X_ABSTRACTVIDEODATAOUTPUT_H

#include "export.h"
#include "../abstractvideooutput.h"
#include "../phonondefs.h"
#include <QtCore/QObject>
#include "videoframe2.h"

#ifndef DOXYGEN_SHOULD_SKIP_THIS
template<typename T> class QSet;
#endif

namespace Phonon
{
namespace Experimental
{

class AbstractVideoDataOutputPrivate;

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
 *
 * \see VideoDataOutput2
 */
class PHONONEXPERIMENTAL_EXPORT AbstractVideoDataOutput : public AbstractVideoOutput
{
    K_DECLARE_PRIVATE(AbstractVideoDataOutput)

    public:
        /**
         * Constructs an AbstractVideoDataOutput
         */
        AbstractVideoDataOutput();
        ~AbstractVideoDataOutput();

        /**
         * Lists the formats this output is allowed to pass via frameReady.
         */
        virtual QSet<VideoFrame2::Format> allowedFormats() const;

        // TODO: consider to remove the following function and make the above pure virtual
        /**
         * Defaults to VideoFrame2::Format_RGB888.
         */
        void setAllowedFormats(const QSet<VideoFrame2::Format> &);

        bool isRunning() const;

        /**
         * This function is called whenever a frame should be displayed.
         *
         * \warning frameReady can get called from any thread (other than the
         * main thread or the thread affinity of this QObject). A common error to create a deadlock
         * situation is to block the thread this function is called from, until the main thread
         * has found time to handle the frame. If it is blocking while the main thread decides to
         * stop/delete the MediaObject you might get a deadlock.
         */
        virtual void frameReady(const VideoFrame2 &) = 0;

        /**
         * This function is called after the last frameReady of a MediaObject was called.
         */
        virtual void endOfMedia() = 0;

        void setRunning(bool running);
        void start();
        void stop();

    protected:
        AbstractVideoDataOutput(AbstractVideoDataOutputPrivate &dd);
};

} // namespace Experimental
} //namespace Phonon

#endif // PHONON_X_ABSTRACTVIDEODATAOUTPUT_H
