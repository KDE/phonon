/*
    Copyright (C) 2014 Harald Sitter <sitter@kde.org>

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
#include <QtCore/QStringList>

#include "debug_p.h"
#include "factory_p.h"
#include "backendinterface.h"
#include "phononpimpl_p.h"

namespace Phonon {

class DescriptionBasePrivate
{
public:
    DescriptionBasePrivate()
        : valid(false)
        , name()
        , description()
    {
        pDebug() << Q_FUNC_INFO;
    }

    void init(QString _name, QString _description)
    {
        pDebug() << Q_FUNC_INFO << _name << _description;
        valid = true;
        name = _name;
        description = _description;
    }

    bool operator==(const DescriptionBasePrivate &other) const
    {
        pDebug() << Q_FUNC_INFO;
        return (valid == other.valid) &&
                (name == other.name) &&
                (description == other.description);
    }

    bool operator!=(const DescriptionBasePrivate &other) const
    {
        pDebug() << Q_FUNC_INFO;
        return (valid != other.valid) &&
                (name != other.name) &&
                (description != other.description);
    }

    bool valid;

    QString name;
    QString description;

#warning compat properties
    QHash<QByteArray, QVariant> properties;
};

DescriptionBase::DescriptionBase()
    : k_ptr(new DescriptionBasePrivate())
{
    pDebug() << Q_FUNC_INFO;
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
    pDebug() << Q_FUNC_INFO;
    *(this->k_ptr) = *(other.k_ptr);
    return *this;
}

bool DescriptionBase::operator==(const DescriptionBase &other) const
{
    pDebug() << Q_FUNC_INFO;
    return *(this->k_ptr) == *(other.k_ptr);
}

bool DescriptionBase::operator!=(const DescriptionBase &other) const
{
    pDebug() << Q_FUNC_INFO;
    return *(this->k_ptr) != *(other.k_ptr);
}

DescriptionBase::DescriptionBase(const DescriptionBase &other)
    : k_ptr(new DescriptionBasePrivate(*other.k_ptr))
{
    pDebug() << Q_FUNC_INFO;
}

DescriptionBase::~DescriptionBase()
{
    delete k_ptr;
}

DescriptionBase::DescriptionBase(DescriptionBasePrivate &dd)
    : k_ptr(&dd)
{
    pDebug() << Q_FUNC_INFO;
}

EffectDescription::EffectDescription()
    : DescriptionBase()
{
    pDebug() << Q_FUNC_INFO;
}

EffectDescription::EffectDescription(QString name, QString description)
    : DescriptionBase()
{
    pDebug() << Q_FUNC_INFO;
    P_D(DescriptionBase);
    d->init(name, description);
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
    pDebug() << Q_FUNC_INFO;
}

DeviceDescriptionBase::~DeviceDescriptionBase()
{
    pDebug() << Q_FUNC_INFO;
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
    pDebug() << Q_FUNC_INFO;
}

AudioOutputDevice::AudioOutputDevice(QString name, QString description)
    : DeviceDescriptionBase()
{
    pDebug() << Q_FUNC_INFO;
    P_D(DeviceDescriptionBase);
    d->init(name, description);
}

AudioOutputDevice::~AudioOutputDevice()
{
    pDebug() << Q_FUNC_INFO;
}

AudioCaptureDevice::AudioCaptureDevice()
    : DeviceDescriptionBase()
{
    pDebug() << Q_FUNC_INFO;
}

AudioCaptureDevice::AudioCaptureDevice(QString name, QString description)
    : DeviceDescriptionBase()
{
    pDebug() << Q_FUNC_INFO;
    P_D(DeviceDescriptionBase);
    d->init(name, description);
}

AudioCaptureDevice::~AudioCaptureDevice()
{
    pDebug() << Q_FUNC_INFO;
}

VideoCaptureDevice::VideoCaptureDevice()
    : DeviceDescriptionBase()
{
    pDebug() << Q_FUNC_INFO;
}

VideoCaptureDevice::VideoCaptureDevice(QString name, QString description)
    : DeviceDescriptionBase()
{
    pDebug() << Q_FUNC_INFO;
    P_D(DeviceDescriptionBase);
    d->init(name, description);
}

VideoCaptureDevice::~VideoCaptureDevice()
{
    pDebug() << Q_FUNC_INFO;
}

} // namespace Phonon
