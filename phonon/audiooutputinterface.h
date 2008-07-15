/*  This file is part of the KDE project
    Copyright (C) 2007-2008 Matthias Kretz <kretz@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), Trolltech ASA 
    (or its successors, if any) and the KDE Free Qt Foundation, which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public 
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef PHONON_AUDIOOUTPUTINTERFACE_H
#define PHONON_AUDIOOUTPUTINTERFACE_H

#include "phononnamespace.h"
#include "objectdescription.h"
#include "phonondefs.h"
#include <QtCore/QtGlobal>

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

namespace Phonon
{
/** \class AudioOutputInterface audiooutputinterface.h Phonon/AudioOutputInterface
 * \short Interface for AudioOutput objects
 *
 * \author Matthias Kretz <kretz@kde.org>
 */
class AudioOutputInterface40
{
    public:
        virtual ~AudioOutputInterface40() {}

        /**
         * Returns the current software volume.
         *
         * A value of 0.0 means muted, 1.0 means unchanged, 2.0 means double voltage (i.e. all
         * samples are multiplied by 2).
         */
        virtual qreal volume() const = 0;
        /**
         * Sets the new current software volume.
         *
         * A value of 0.0 means muted, 1.0 means unchanged, 2.0 means double voltage (i.e. all
         * samples are multiplied by 2).
         */
        virtual void setVolume(qreal) = 0;

        /**
         * Returns the index of the device that is used. The index is the number returned from
         * BackendInterface::objectDescriptionIndexes(AudioOutputDeviceType).
         */
        virtual int outputDevice() const = 0;
        /**
         * \deprecated
         *
         * Requests to change the current output device to the one identified by the passed index.
         *
         * The index is the number returned from
         * BackendInterface::objectDescriptionIndexes(AudioOutputDeviceType).
         *
         * \returns \c true if the requested device works and is used after this call.
         * \returns \c false if something failed and the device is not used after this call.
         */
        virtual bool setOutputDevice(int) = 0;
};

class AudioOutputInterface42 : public AudioOutputInterface40
{
    public:
        /**
         * Requests to change the current output device.
         *
         * \returns \c true if the requested device works and is used after this call.
         * \returns \c false if something failed and the device is not used after this call.
         */
        virtual bool setOutputDevice(const Phonon::AudioOutputDevice &) = 0;

        using AudioOutputInterface40::setOutputDevice;
};

} // namespace Phonon

#ifdef PHONON_BACKEND_VERSION_4_2
namespace Phonon { typedef AudioOutputInterface42 AudioOutputInterface; }
Q_DECLARE_INTERFACE(Phonon::AudioOutputInterface40, "AudioOutputInterface2.phonon.kde.org")
Q_DECLARE_INTERFACE(Phonon::AudioOutputInterface,   "3AudioOutputInterface.phonon.kde.org")
#else
namespace Phonon { typedef AudioOutputInterface40 AudioOutputInterface; }
Q_DECLARE_INTERFACE(Phonon::AudioOutputInterface,   "AudioOutputInterface2.phonon.kde.org")
Q_DECLARE_INTERFACE(Phonon::AudioOutputInterface42, "3AudioOutputInterface.phonon.kde.org")
#endif

QT_END_NAMESPACE
QT_END_HEADER

#endif // PHONON_AUDIOOUTPUTINTERFACE_H
