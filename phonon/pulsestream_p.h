/*
    Copyright (C) 2010 Colin Guthrie <cguthrie@mandriva.org>
    Copyright (C) 2013 Harald Sitter <sitter@kde.org>

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

#ifndef PHONON_PULSESTREAM_P_H
#define PHONON_PULSESTREAM_P_H

#include <QtCore/QObject>

#include <pulse/pulseaudio.h>
#include <stdint.h>

#include "phonon_export.h"
#include "phononnamespace.h"


namespace Phonon
{
    class PHONON_EXPORT PulseStream : public QObject
    {
        Q_OBJECT
        public:
            PulseStream(QString streamUuid, QString role);
            ~PulseStream();

            QString uuid() const;

            uint32_t index() const;
            void setIndex(uint32_t index);

            uint8_t channels() const;

            void setDevice(int device);
            void setVolume(const pa_cvolume *volume);
            void setMute(bool mute);

            qreal cachedVolume() const;
            void setCachedVolume(qreal volume);

            QString role() const;

        signals:
            void usingDevice(int device);
            void volumeChanged(qreal volume);
            void muteChanged(bool mute);

    private slots:
            void applyCachedVolume();

        private:
            QString mStreamUuid;
            uint32_t mIndex;
            int mDevice;
            pa_cvolume mVolume;
            bool mMute;
            // -1 = not set
            qreal mCachedVolume;
            QString mRole;
    };
} // namespace Phonon


#endif // PHONON_PULSESTREAM_P_H
