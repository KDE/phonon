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

#ifndef PHONON_VIDEOGRAPHICSOBJECTINTERFACE_H
#define PHONON_VIDEOGRAPHICSOBJECTINTERFACE_H

namespace Phonon {

class VideoGraphicsObjectInterface
{
public:
    /// Destructor.
    virtual ~VideoGraphicsObjectInterface() {}

    /// Lock video frame.
    virtual void lock() = 0;

    /**
     * Try to lock video frame.
     * \returns whether locking was successful.
     */
    virtual bool tryLock() = 0;

    /// Unlock video frame.
    virtual void unlock() = 0;

    /**
     * Access the most current video frame.
     * Please mind that an internal mutex must be locked/unlocked accordingly.
     * \returns the current frame (held in the interface implementation)
     */
    virtual const VideoFrame *frame() const = 0;

    // Signals

    /// Signal to be emitted when a new frame is ready for painting.
    virtual void frameReady() = 0;

    /// Signal to be emitted when the frontend object should reset (Painters for instance).
    virtual void reset() = 0;
};

} // namespace Phonon

Q_DECLARE_INTERFACE(Phonon::VideoGraphicsObjectInterface,
                    "VideoGraphicsObjectInterface.phonon.kde.org")

#endif // PHONON_VIDEOGRAPHICSOBJECTINTERFACE_H
