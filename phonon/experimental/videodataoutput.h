/*  This file is part of the KDE project
    Copyright (C) 2005-2007 Matthias Kretz <kretz@kde.org>

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

#ifndef PHONON_VIDEODATAOUTPUT_H
#define PHONON_VIDEODATAOUTPUT_H

#include "export.h"
#include "../abstractvideooutput.h"
#include "../phonondefs.h"
#include <QtCore/QObject>

#ifndef DOXYGEN_SHOULD_SKIP_THIS
template<typename T> class QVector;
template<typename Key, typename T> class QMap;
#endif
class QSize;

namespace Phonon
{
namespace Experimental
{
    class VideoDataOutputPrivate;
    struct VideoFrame;

    /**
     * \short This class gives you nothing. ;-)
     * \deprecated
     *
     * \author Matthias Kretz <kretz@kde.org>
     */
    class PHONONEXPERIMENTAL_EXPORT VideoDataOutput : public QObject, public AbstractVideoOutput
    {
        Q_OBJECT
        K_DECLARE_PRIVATE(VideoDataOutput)
        /**
         * This property retrieves the nominal latency of the
         * backend.
         */
        Q_PROPERTY(int latency READ latency)

        /**
         * This property indicates the state of the data output.
         */
        Q_PROPERTY(bool running READ isRunning WRITE setRunning)

        PHONON_HEIR(VideoDataOutput)
    public:
        int latency() const;

        bool isRunning() const;

        Phonon::Experimental::VideoFrame frameForTime(qint64 timestamp);

    public Q_SLOTS:
        void setRunning(bool running);
        void start();
        void stop();

    Q_SIGNALS:
        /* FIXME: disabled this piece of documentation - add another * to enable
         * Fixme: I don't think this makes sense, but I've been wrong before.
         *
         * Emitted whenever another dataSize number of samples are ready and
         * format is set to IntegerFormat.
         *
         * If format is set to FloatFormat the signal is not emitted at all.
         *
         * \param frame An object of class VideoFrame holding the video data
         * and some additional information.
         * void frameReady(const Phonon::Experimental::VideoFrame &frame);
         */

        /**
         * The signal is emitted whenever a frame should be displayed.
         *
         * The relevant frames should be fetched and displayed using frameForTime
         * method.
         *
         * \param nowStamp the current time
         * \param outStamp the time the frame should be displayed with
         */
        void displayFrame(qint64 nowStamp, qint64 outStamp);

        /**
         * This signal is emitted after the last frameReady signal of a
         * media is emitted.
         */
        void endOfMedia();
    };

} // namespace Experimental
} //namespace Phonon

// vim: sw=4 ts=4 tw=80
#endif // PHONON_VIDEODATAOUTPUT_H
