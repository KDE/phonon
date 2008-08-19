/*  This file is part of the KDE project
    Copyright (C) 2007 Matthias Kretz <kretz@kde.org>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.

*/

#ifndef PHONON_XINE_CONNECTNOTIFICATIONINTERFACE_H
#define PHONON_XINE_CONNECTNOTIFICATIONINTERFACE_H

namespace Phonon
{
namespace Xine
{

class ConnectNotificationInterface
{
    public:
        virtual ~ConnectNotificationInterface() {}
        /**
         * This function is called before the xine thread is issued the rewire commands
         */
        virtual void graphChanged() = 0;
};

} // namespace Xine
} // namespace Phonon

Q_DECLARE_INTERFACE(Phonon::Xine::ConnectNotificationInterface, "XineConnectNotificationInterface.phonon.kde.org")

#endif // PHONON_XINE_CONNECTNOTIFICATIONINTERFACE_H
