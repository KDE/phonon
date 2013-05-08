/*
    Copyright (C) 2005-2007 Matthias Kretz <kretz@kde.org>
    Copyright (C) 2011 Trever Fischer <tdfischer@kde.org>
    Copyright (C) 2013 Harald Sitter <sitter@kde.org

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

#ifndef PHONON_AUDIOOUTPUTINTERFACE_H
#define PHONON_AUDIOOUTPUTINTERFACE_H

#include "phononnamespace.h"
#include "objectdescription.h"
#include "phonondefs.h"
#include <QtCore/QtGlobal>

namespace Phonon {

class AudioOutputInterface
{
    public:
        virtual ~AudioOutputInterface() {}
        virtual qreal volume() const = 0;
        virtual void setVolume(qreal) = 0;
        virtual int outputDevice() const = 0;
        virtual bool setOutputDevice(const Phonon::AudioOutputDevice &) = 0;
};

} // namespace Phonon

Q_DECLARE_INTERFACE(Phonon::AudioOutputInterface, "AudioOutputInterface.phonon.kde.org/5.0")

#endif // PHONON_AUDIOOUTPUTINTERFACE_H
