/*  This file is part of the KDE project
    Copyright (C) 2005-2006 Matthias Kretz <kretz@kde.org>

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

#include "backendcapabilities.h"
#include "backendcapabilities_p.h"

#include "phonondefs_p.h"
#include "backendinterface.h"
#include "factory_p.h"
#include "globalconfig_p.h"
#include "globalstatic_p.h"
#include "objectdescription.h"

#include <QtCore/QList>
#include <QtCore/QSet>
#include <QtCore/QStringList>

QT_BEGIN_NAMESPACE

PHONON_GLOBAL_STATIC(Phonon::BackendCapabilitiesPrivate, globalBCPrivate)

namespace Phonon
{

BackendCapabilities::Notifier *BackendCapabilities::notifier()
{
    return globalBCPrivate;
}

QStringList BackendCapabilities::availableMimeTypes()
{
    if (BackendInterface *backendIface = qobject_cast<BackendInterface *>(Factory::backend()))
        return backendIface->availableMimeTypes();
    else
        return QStringList();
}

bool BackendCapabilities::isMimeTypeAvailable(const QString &mimeType)
{
    QObject *m_backendObject = Factory::backend(false);
    if (!m_backendObject) {
        if (!Factory::isMimeTypeAvailable(mimeType)) {
            return false;
        }
        // without loading the backend we found out that the MIME type might be supported, now we
        // want to know for certain. For that we need to load the backend.
        m_backendObject = Factory::backend(true);
    }
    if (!m_backendObject) {
        // no backend == no MIME type supported at all
        return false;
    }
    return availableMimeTypes().contains(mimeType);
}

QList<AudioOutputDevice> BackendCapabilities::availableAudioOutputDevices()
{
    QList<AudioOutputDevice> ret;
    const QList<int> deviceIndexes = GlobalConfig().audioOutputDeviceListFor(Phonon::NoCategory);
    foreach (int i, deviceIndexes) {
        ret.append(AudioOutputDevice::fromIndex(i));
    }
    return ret;
}


#ifndef QT_NO_PHONON_AUDIOCAPTURE
QList<AudioCaptureDevice> BackendCapabilities::availableAudioCaptureDevices()
{
    QList<AudioCaptureDevice> ret;
    const QList<int> deviceIndexes = GlobalConfig().audioCaptureDeviceListFor(Phonon::NoCategory);
    foreach (int i, deviceIndexes) {
        ret.append(AudioCaptureDevice::fromIndex(i));
    }
    return ret;
}
#endif //QT_NO_PHONON_AUDIOCAPTURE

#ifndef QT_NO_PHONON_EFFECT
QList<EffectDescription> BackendCapabilities::availableAudioEffects()
{
    BackendInterface *backendIface = qobject_cast<BackendInterface *>(Factory::backend());
    QList<EffectDescription> ret;
    if (backendIface) {
        QList<int> deviceIndexes = backendIface->objectDescriptionIndexes(Phonon::EffectType);
        foreach (int i, deviceIndexes) {
            ret.append(EffectDescription::fromIndex(i));
        }
    }
    return ret;
}
#endif //QT_NO_PHONON_EFFECT

} // namespace Phonon

QT_END_NAMESPACE

#include "moc_backendcapabilities.cpp"

// vim: sw=4 ts=4


