/*  This file is part of the KDE project
    Copyright (C) 2007 Matthias Kretz <kretz@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License version 2 as published by the Free Software Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.

*/

#include "platform_p.h"
#include "platformplugin.h"
#include "factory_p.h"
#include <QtCore/QCoreApplication>
#include <QtCore/QUrl>
#include <QtGui/QIcon>
#include <QtGui/QStyle>
#include <QtGui/QApplication>

QT_BEGIN_NAMESPACE

namespace Phonon
{

void Platform::saveVolume(const QString &outputName, qreal volume)
{
#ifndef QT_NO_PHONON_PLATFORMPLUGIN
    PlatformPlugin *f = Factory::platformPlugin();
    if (f) {
        f->saveVolume(outputName, volume);
    }
#else
    Q_UNUSED(outputName);
    Q_UNUSED(volume);
#endif //QT_NO_PHONON_PLATFORMPLUGIN
}

qreal Platform::loadVolume(const QString &outputName)
{
#ifndef QT_NO_PHONON_PLATFORMPLUGIN
    const PlatformPlugin *f = Factory::platformPlugin();
    if (f) {
        return f->loadVolume(outputName);
    }
#else
    Q_UNUSED(outputName);
#endif //QT_NO_PHONON_PLATFORMPLUGIN
    return 1.0;
}

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

QIcon Platform::icon(const QString &name, QStyle *style)
{
    QIcon ret;
#ifndef QT_NO_PHONON_PLATFORMPLUGIN
    if (const PlatformPlugin *f = Factory::platformPlugin()) {
        ret = f->icon(name);
    }
#endif //QT_NO_PHONON_PLATFORMPLUGIN
    if (ret.isNull()) {
        if (!style) {
            style = QApplication::style();
        }
        if (name == QLatin1String("player-volume")) {
            ret = style->standardPixmap(QStyle::SP_MediaVolume);
        } else if (name == QLatin1String("player-volume-muted")) {
            ret = style->standardPixmap(QStyle::SP_MediaVolumeMuted);
        }
    }

    return ret;
}

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

QString Platform::applicationName()
{
#ifndef QT_NO_PHONON_PLATFORMPLUGIN
    const PlatformPlugin *f = Factory::platformPlugin();
    if (f) {
        return f->applicationName();
    }
#endif //QT_NO_PHONON_PLATFORMPLUGIN
    QString ret = QCoreApplication::applicationName();
    if (ret.isEmpty())
        ret = QCoreApplication::applicationFilePath();
    return ret;
}

} // namespace Phonon

QT_END_NAMESPACE
