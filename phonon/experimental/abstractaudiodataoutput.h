/*  This file is part of the KDE project
    Copyright (C) 2005-2006,2008 Matthias Kretz <kretz@kde.org>

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
#ifndef PHONON_X_ABSTRACTAUDIODATAOUTPUT_H
#define PHONON_X_ABSTRACTAUDIODATAOUTPUT_H

#include "export.h"
#include "../medianode.h"
#include "../phonondefs.h"
#include "audioformat.h"

#ifndef DOXYGEN_SHOULD_SKIP_THIS
template<typename T> class QSet;
#endif

namespace Phonon
{
namespace Experimental
{

class Packet;
class AbstractAudioDataOutputPrivate;

/**
 * \short This class gives you the audio data.
 *
 * This class implements a special AbstractAudioOutput that gives your
 * application the audio data. This class is usable for realtime performance.
 *
 * \author Matthias Kretz <kretz@kde.org>
 */
class PHONONEXPERIMENTAL_EXPORT AbstractAudioDataOutput : public Phonon::MediaNode
{
    K_DECLARE_PRIVATE(AbstractAudioDataOutput)
    public:
        /**
         * Constructs an AbstractAudioDataOutput
         */
        AbstractAudioDataOutput();
        ~AbstractAudioDataOutput();

        /**
         * Lists the formats this output is allowed to pass via frameReady.
         */
        virtual QSet<AudioFormat> allowedFormats() const;

        // TODO: consider to remove the following function and make the above pure virtual
        /**
         * Defaults to AudioFormat().
         * I.e. 16 bit, stereo, 48000 kHz PCM data.
         */
        void setAllowedFormats(const QSet<AudioFormat> &);

        bool isRunning() const;

        /**
         * This function is called whenever an audio packet is ready for
         * processing.
         *
         * \warning packetReady can get called from any thread (other than the
         * main thread or the thread affinity of this QObject).
         */
        virtual void packetReady(const Packet &) = 0;

        /**
         * This function is called after the last packetReady of a media was
         * called.
         */
        void endOfMedia();

        void setRunning(bool running);
        void start();
        void stop();

    protected:
        AbstractAudioDataOutput(AbstractAudioDataOutputPrivate &dd);
};

} // namespace Experimental
} // namespace Phonon

#endif // PHONON_X_ABSTRACTAUDIODATAOUTPUT_H
