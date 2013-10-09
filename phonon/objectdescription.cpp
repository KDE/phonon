/*  This file is part of the KDE project
    Copyright (C) 2006-2008 Matthias Kretz <kretz@kde.org>

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

#include "objectdescription.h"
#include "objectdescription_p.h"

#include <QtCore/QObject>
#include <QtCore/QSet>
#include "factory_p.h"
#include <QtCore/QStringList>
#include "backendinterface.h"
#include "platformplugin.h"
#include "pulsesupport.h"
#include "phononpimpl_p.h"

namespace Phonon
{

class DescriptionBasePrivate
{
public:
    DescriptionBasePrivate()
        : valid(false)
        , index(-1)
        , name()
        , description()
    {
        qDebug() << Q_FUNC_INFO;
    }

    void init(int _index, QString _name, QString _description)
    {
        qDebug() << Q_FUNC_INFO << _index << _name << _description;
        valid = true;
        index = _index;
        name = _name;
        description = _description;
    }

    bool operator==(const DescriptionBasePrivate &other) const
    {
        qDebug() << Q_FUNC_INFO;
        return (valid == other.valid) &&
                (index == other.index) &&
                (name == other.name) &&
                (description == other.description);
    }

    bool operator!=(const DescriptionBasePrivate &other) const
    {
        qDebug() << Q_FUNC_INFO;
        return (valid != other.valid) &&
                (index != other.index) &&
                (name != other.name) &&
                (description != other.description);
    }

    bool valid;

    int index;
    QString name;
    QString description;

#warning compat properties
    QHash<QByteArray, QVariant> properties;
};

DescriptionBase::DescriptionBase()
    : k_ptr(new DescriptionBasePrivate())
{
    qDebug() << Q_FUNC_INFO;
}

int DescriptionBase::index() const
{
    P_D(const DescriptionBase);
    return d->index;
}

QString DescriptionBase::name() const
{
    P_D(const DescriptionBase);
    return d->name;
}

QString DescriptionBase::description() const
{
    P_D(const DescriptionBase);
    return d->description;
}

bool DescriptionBase::isValid() const
{
    P_D(const DescriptionBase);
    return d->valid;
}

DescriptionBase &DescriptionBase::operator=(const DescriptionBase &other)
{
    qDebug() << Q_FUNC_INFO;
    *(this->k_ptr) = *(other.k_ptr);
    return *this;
}

bool DescriptionBase::operator==(const DescriptionBase &other) const
{
    qDebug() << Q_FUNC_INFO;
    return *(this->k_ptr) == *(other.k_ptr);
}

bool DescriptionBase::operator!=(const DescriptionBase &other) const
{
    qDebug() << Q_FUNC_INFO;
    return *(this->k_ptr) != *(other.k_ptr);
}

DescriptionBase::DescriptionBase(const DescriptionBase &other)
    : k_ptr(new DescriptionBasePrivate(*other.k_ptr))
{
    qDebug() << Q_FUNC_INFO;
}

DescriptionBase::~DescriptionBase()
{
    delete k_ptr;
}

DescriptionBase::DescriptionBase(DescriptionBasePrivate &dd)
    : k_ptr(&dd)
{
    qDebug() << Q_FUNC_INFO;
}

EffectDescription::EffectDescription()
    : DescriptionBase()
{
    qDebug() << Q_FUNC_INFO;
}

EffectDescription::EffectDescription(int index, QString name, QString description)
    : DescriptionBase()
{
    qDebug() << Q_FUNC_INFO;
    P_D(DescriptionBase);
    d->init(index, name, description);
}

class DeviceDescriptionBasePrivate : public DescriptionBasePrivate
{
public:
    DeviceDescriptionBasePrivate()
        : available(false)
    {
    }

    bool available;
};

DeviceDescriptionBase::DeviceDescriptionBase()
    : DescriptionBase(*new DeviceDescriptionBasePrivate())
{
    qDebug() << Q_FUNC_INFO;
}

DeviceDescriptionBase::~DeviceDescriptionBase()
{
    qDebug() << Q_FUNC_INFO;
}

bool DeviceDescriptionBase::isAvailable() const
{
    P_D(const DeviceDescriptionBase);
    return d->available;
}

#warning property compat function
QVariant DeviceDescriptionBase::property(const char *name) const
{
    qWarning() << "Received call to" << Q_FUNC_INFO << "requesting" << name;
    P_D(const DescriptionBase);
    return d->properties.value(name);
}

#warning propertynames compat function
QList<QByteArray> DeviceDescriptionBase::propertyNames() const
{
    qWarning() << "Received call to" << Q_FUNC_INFO;
    P_D(const DescriptionBase);
    return d->properties.keys();
}

AudioOutputDevice::AudioOutputDevice()
    : DeviceDescriptionBase()
{
    qDebug() << Q_FUNC_INFO;
}

AudioOutputDevice::AudioOutputDevice(int index, QString name, QString description)
    : DeviceDescriptionBase()
{
    qDebug() << Q_FUNC_INFO;
    P_D(DeviceDescriptionBase);
    d->init(index, name, description);
}

AudioOutputDevice::~AudioOutputDevice()
{
    qDebug() << Q_FUNC_INFO;
}

AudioOutputDevice AudioOutputDevice::fromIndex(int index)
{
    bool is_audio_device = true;

//    PulseSupport *pulse = PulseSupport::getInstance();
//    if (is_audio_device && pulse->isActive()) {
//        QList<int> indexes = pulse->objectDescriptionIndexes(type);
//        if (indexes.contains(index)) {
//            QHash<QByteArray, QVariant> properties = pulse->objectDescriptionProperties(type, index);
//            return new ObjectDescriptionData(index, properties);
//        }

//        // When Pulse is enabled, only try from the platform plugin or backend if it is about audio capture
//        if (type != AudioCaptureDeviceType)
//            return new ObjectDescriptionData(0); // invalid
//    }

//#ifndef QT_NO_PHONON_PLATFORMPLUGIN
//    // prefer to get the ObjectDescriptionData from the platform plugin
//    PlatformPlugin *platformPlugin = Factory::platformPlugin();
//    if (platformPlugin) {
//        QList<int> indexes = platformPlugin->objectDescriptionIndexes(type);
//        if (indexes.contains(index)) {
//            QHash<QByteArray, QVariant> properties = platformPlugin->objectDescriptionProperties(type, index);
//            return new ObjectDescriptionData(index, properties);
//        }
//    }
//#endif //QT_NO_PHONON_PLATFORMPLUGIN

    BackendInterface *iface = qobject_cast<BackendInterface *>(Factory::backend());
    if (iface) {
        QList<int> indexes = iface->objectDescriptionIndexes(AudioOutputDeviceType);
        if (indexes.contains(index)) {
            QHash<QByteArray, QVariant> properties = iface->objectDescriptionProperties(AudioOutputDeviceType, index);
            AudioOutputDevice dev(index, properties.value("name").toString(), properties.value("description").toString());
            dev.k_func()->available = properties.value("available").toBool();
            dev.k_func()->properties = properties;
            return dev;
//            return new ObjectDescriptionData(index, properties);
        }
    }

    return AudioOutputDevice();
//    return new ObjectDescriptionData(0); // invalid
}

AudioCaptureDevice::AudioCaptureDevice()
    : DeviceDescriptionBase()
{
    qDebug() << Q_FUNC_INFO;
}

AudioCaptureDevice::AudioCaptureDevice(int index, QString name, QString description)
    : DeviceDescriptionBase()
{
    qDebug() << Q_FUNC_INFO;
    P_D(DeviceDescriptionBase);
    d->init(index, name, description);
}

AudioCaptureDevice::~AudioCaptureDevice()
{
    qDebug() << Q_FUNC_INFO;
}

VideoCaptureDevice::VideoCaptureDevice()
    : DeviceDescriptionBase()
{
    qDebug() << Q_FUNC_INFO;
}

VideoCaptureDevice::VideoCaptureDevice(int index, QString name, QString description)
    : DeviceDescriptionBase()
{
    qDebug() << Q_FUNC_INFO;
    P_D(DeviceDescriptionBase);
    d->init(index, name, description);
}

VideoCaptureDevice::~VideoCaptureDevice()
{
    qDebug() << Q_FUNC_INFO;
}

} //namespace Phonon
