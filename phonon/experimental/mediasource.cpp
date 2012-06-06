/*  This file is part of the KDE project
    Copyright (C) 2008 Matthias Kretz <kretz@kde.org>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of
    the License or (at your option) version 3 or any later version
    accepted by the membership of KDE e.V. (or its successor approved
    by the membership of KDE e.V.), Nokia Corporation (or its successors, 
    if any) and the KDE Free Qt Foundation, which shall act as a proxy 
    defined in Section 14 of version 3 of the license.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "mediasource.h"
#include "mediasource_p.h"

#define S_D(Class) Class##Private *d = reinterpret_cast<Class##Private *>(Phonon::MediaSource::d.data())

namespace Phonon
{
namespace Experimental
{

MediaSource::MediaSource(const MediaSource &rhs)
    : Phonon::MediaSource(rhs)
{
}

MediaSource::MediaSource(const QList<Phonon::MediaSource> &mediaList)
    : Phonon::MediaSource(*new MediaSourcePrivate(Link))
{
    S_D(MediaSource);
    d->linkedSources = mediaList;
    foreach (const Phonon::MediaSource &ms, mediaList) {
        Q_ASSERT(static_cast<MediaSource::Type>(ms.type()) != Link);
        Q_UNUSED(ms);
    }
}

#ifndef PHONON_NO_VIDEOCAPTURE
MediaSource::MediaSource(const VideoCaptureDevice &videoDevice)
    : Phonon::MediaSource(*new MediaSourcePrivate(VideoCaptureDeviceSource))
{
    Q_UNUSED(videoDevice);
}
#endif // PHONON_NO_VIDEOCAPTURE

MediaSource &MediaSource::operator=(const MediaSource &rhs)
{
    d = rhs.d;
    return *this;
}

bool MediaSource::operator==(const MediaSource &rhs) const
{
    return d == rhs.d;
}

#ifndef PHONON_NO_VIDEOCAPTURE
VideoCaptureDevice MediaSource::videoCaptureDevice() const
{
    return phononVcdToExperimentalVcd(Phonon::MediaSource::videoCaptureDevice());
}
#endif // PHONON_NO_VIDEOCAPTURE

QList<Phonon::MediaSource> MediaSource::substreams() const
{
    S_D(MediaSource);
    return d->linkedSources;
}

} // namespace Experimental
} // namespace Phonon
