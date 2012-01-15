/*  This file is part of the KDE project
    Copyright (C) 2008 Matthias Kretz <kretz@kde.org>

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

#ifndef PHONON_X_AUDIOFORMAT_H
#define PHONON_X_AUDIOFORMAT_H

#include <QtCore/QtGlobal>
#include "phononnamespace.h"

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

namespace Phonon
{
namespace Experimental
{

class AudioFormatPrivate;
class AudioFormat
{
    //Q_DECLARE_PRIVATE(AudioFormat)
    public:
        AudioFormat(int sampleRate = 48000, int channelCount = 2, Phonon::Experimental::BitRate bitRate = Phonon::Experimental::Signed16Bit, QSysInfo::Endian byteOrder = QSysInfo::ByteOrder);
        AudioFormat(const AudioFormat &);
        ~AudioFormat();

        AudioFormat &operator=(const AudioFormat &);

        int sampleRate() const;
        int channelCount() const;
        Phonon::Experimental::BitRate bitRate() const;
        QSysInfo::Endian byteOrder() const;

        bool operator==(const AudioFormat &) const;
        inline bool operator!=(const AudioFormat &f) const { return !operator==(f); }

        /**
         * ess than operator for sorting:
         * A Format is considered smaller if
         * - smaller bit rate
         * - smaller sample rate
         * - less channels
         * - non-native byte order
         */
        bool operator<(const AudioFormat &) const;

        quint32 key() const;

    private:
        union
        {
            struct
            {
                int m_sampleRate;
                int m_channelCount;
                Phonon::Experimental::BitRate m_bitRate;
                QSysInfo::Endian m_byteOrder;
            } s;
            // for future use:
            AudioFormatPrivate *d_ptr;
        };
};

inline uint qHash(const AudioFormat &p)
{
    return p.key();
}

} // namespace Experimental
} // namespace Phonon

#if defined(Q_CC_MSVC) && _MSC_VER <= 1300
//this ensures that code outside Phonon can use the hash function
//it also a workaround for some compilers
inline uint qHash(const Phonon::Experimental::AudioFormat &p) { return Phonon::Experimental::qHash(p); } //krazy:exclude=inline
#endif

QT_END_NAMESPACE
QT_END_HEADER

#endif // PHONON_X_AUDIOFORMAT_H
