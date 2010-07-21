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

#include "factory_p.h"
#include "objectdescription.h"
#include "../factory_p.h"
#include "../globalstatic_p.h"
#include "../backendinterface.h"
#include "backendinterface.h"
#include <QtCore/QtDebug>

namespace Phonon
{
namespace Experimental
{

class FactoryPrivate : public Phonon::Experimental::Factory::Sender
{
    public:
        FactoryPrivate();
        ~FactoryPrivate();
        //QPointer<QObject> m_backendObject;

    private Q_SLOTS:
        void objectDescriptionChanged(ObjectDescriptionType);
};

PHONON_GLOBAL_STATIC(Phonon::Experimental::FactoryPrivate, globalFactory)

FactoryPrivate::FactoryPrivate()
{
    QObject *backendObj = Phonon::Factory::backend();
    Q_ASSERT(backendObj);
    //QMetaObject::invokeMethod(backendObj, "experimentalBackend", Qt::DirectConnection,
            //Q_RETURN_ARG(QObject *, m_backendObject));
    //if (!m_backendObject) {
        //qDebug() << "The backend does not support Phonon::Experimental";
        //return;
    //}
    connect(backendObj, SIGNAL(objectDescriptionChanged(ObjectDescriptionType)),
            SLOT(objectDescriptionChanged(ObjectDescriptionType)));
    connect(Phonon::Factory::sender(), SIGNAL(availableVideoCaptureDevicesChanged()), Factory::sender(),
            SLOT(availableVideoCaptureDevicesChanged()));
}

FactoryPrivate::~FactoryPrivate()
{
}

void FactoryPrivate::objectDescriptionChanged(ObjectDescriptionType type)
{
    qDebug() << Q_FUNC_INFO << type;
    switch (type) {
    default:
        break;
    }
}

Factory::Sender *Factory::sender()
{
    return globalFactory;
}

QObject *Factory::createAudioDataOutput(QObject *parent)
{
    Phonon::BackendInterface *b = qobject_cast<Phonon::BackendInterface *>(Phonon::Factory::backend());
    if (b) {
        return Phonon::Factory::registerQObject(b->createObject(
                    static_cast<Phonon::BackendInterface::Class>(Phonon::BackendInterface::AudioDataOutputClass),
                    parent));
    }
    return 0;
}

QObject *Factory::createVideoDataOutput(QObject *parent)
{
    Phonon::BackendInterface *b = qobject_cast<Phonon::BackendInterface *>(Phonon::Factory::backend());
    if (b) {
        return Phonon::Factory::registerQObject(b->createObject(
                    static_cast<Phonon::BackendInterface::Class>(Phonon::BackendInterface::VideoDataOutputClass),
                    parent));
    }
    return 0;
}

QObject *Factory::createAvCapture(QObject *parent)
{
    Phonon::BackendInterface *b = qobject_cast<Phonon::BackendInterface *>(Phonon::Factory::backend());
    if (b) {
        return Phonon::Factory::registerQObject(b->createObject(
                    static_cast<Phonon::BackendInterface::Class>(Phonon::Experimental::BackendInterface::AvCaptureClass),
                    parent));
    }
    return 0;
}

QObject *Factory::createVisualization(QObject *parent)
{
    Phonon::BackendInterface *b = qobject_cast<Phonon::BackendInterface *>(Phonon::Factory::backend());
    if (b) {
        return Phonon::Factory::registerQObject(b->createObject(
                    static_cast<Phonon::BackendInterface::Class>(Phonon::Experimental::BackendInterface::VisualizationClass),
                    parent));
    }
    return 0;
}

} // namespace Experimental
} // namespace Phonon
