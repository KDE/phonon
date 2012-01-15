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

#ifndef PHONON_AUDIOWRITER_H
#define PHONON_AUDIOWRITER_H

#include "export.h"
#include "../abstractaudiooutput.h"
#include "../phonondefs.h"

namespace Phonon
{
namespace Experimental
{

class AudioWriterPrivate;
class PHONONEXPERIMENTAL_EXPORT AudioWriter : public AbstractAudioOutput
{
    Q_OBJECT
    K_DECLARE_PRIVATE(AudioWriter)
    PHONON_HEIR(AudioWriter)
    /**
     * This property defines the codec to be used for encoding the audio signal.
     * Possible codecs can be retrieved from BackendCapabilities using the
     * availableAudioCodecs() function.
     *
     * The default codec is an invalid codec (audioCodec().isValid() == false)
     */
    Q_PROPERTY(AudioCodec audioCodec READ audioCodec WRITE setAudioCodec)
    public:
        AudioCodec audioCodec() const;
        
    public Q_SLOTS:
        setAudioCodec(AudioCodec audioCodec);
        // codec parameters? reuse EffectParameter class?
};

} // namespace Experimental
} // namespace Phonon

#endif // PHONON_AUDIOWRITER_H
// vim: sw=4 ts=4 tw=80
