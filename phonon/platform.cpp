/*
    Copyright (C) 2007 Matthias Kretz <kretz@kde.org>
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

#include "platform_p.h"

#include "platformplugin.h"
#include "factory_p.h"

namespace Phonon {

#warning move KIO construction into consumer app? (kdelibs/runtime?)
AbstractMediaStream *Platform::createMediaStream(const QUrl &url, QObject *parent)
{
#ifndef QT_NO_PHONON_PLATFORMPLUGIN
    PlatformPlugin *f = Factory::platformPlugin();
    if (f) {
        return f->createMediaStream(url, parent);
    }
#else
    Q_UNUSED(url);
    Q_UNUSED(parent);
#endif //QT_NO_PHONON_PLATFORMPLUGIN
    return 0;
}

#warning move notifications to runtime?
void Platform::notification(const char *notificationName, const QString &text,
        const QStringList &actions, QObject *receiver,
        const char *actionSlot)
{
#ifndef QT_NO_PHONON_PLATFORMPLUGIN
    const PlatformPlugin *f = Factory::platformPlugin();
    if (f) {
        f->notification(notificationName, text, actions, receiver, actionSlot);
    }
#else
    Q_UNUSED(notificationName);
    Q_UNUSED(text);
    Q_UNUSED(actions);
    Q_UNUSED(receiver);
    Q_UNUSED(actionSlot);
#endif //QT_NO_PHONON_PLATFORMPLUGIN
}

QList<QPair<QByteArray, QString> > Platform::deviceAccessListFor(const Phonon::AudioOutputDevice &deviceDesc)
{
#ifndef QT_NO_PHONON_PLATFORMPLUGIN
    const PlatformPlugin *f = Factory::platformPlugin();
    if (f) {
        return f->deviceAccessListFor(deviceDesc);
    }
#endif //QT_NO_PHONON_PLATFORMPLUGIN
    return QList<QPair<QByteArray, QString> >();
}

} // namespace Phonon
