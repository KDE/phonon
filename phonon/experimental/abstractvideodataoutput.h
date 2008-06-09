/*  This file is part of the KDE project
    Copyright (C) 2005-2008 Matthias Kretz <kretz@kde.org>

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
 * \see VideoDataOutput
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

    public:
        /**
         * Lists the formats this output is allowed to pass via frameReady.
         */
        QSet<VideoFrame2::Format> allowedFormats() const;

        /**
         * Defaults to VideoFrame2::Format_RGB888.
         */
        void setAllowedFormats(const QSet<VideoFrame2::Format> &);

        bool isRunning() const;

        /**
         * This function is called whenever a frame should be displayed.
         *
         * \warning frameReady can get called from any thread (other than the
         * main thread or the thread affinity of this QObject).
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
