/*
    Copyright (C) 2011 Harald Sitter <sitter@kde.org>

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

#include "backend.h"

#include <QtCore/QStringList>

#include "audiooutput.h"
#include "mediaobject.h"

namespace Phonon {
namespace Fake {

Backend::Backend(QObject *parent) :
    QObject(parent)
{
}

Backend::~Backend() {}

QObject *Backend::createObject(Class c, QObject *parent, const QList<QVariant> &args)
{
    switch (c) {
    case MediaObjectClass:
        return new MediaObject(parent);
//    case VolumeFaderEffectClass:
    case AudioOutputClass:
        return new AudioOutput(parent);
//    case AudioDataOutputClass:
//    case VisualizationClass:
//    case VideoDataOutputClass:
//    case EffectClass:
//    case VideoWidgetClass:
    }
    return 0;
}

QList<int> Backend::objectDescriptionIndexes(ObjectDescriptionType type) const
{
#warning
    return QList<int>();
}

QHash<QByteArray, QVariant> Backend::objectDescriptionProperties(ObjectDescriptionType type, int index) const
{
#warning
    return QHash<QByteArray, QVariant>();
}

bool Backend::startConnectionChange(QSet<QObject *> objects)
{
#warning
    return true;
}

bool Backend::endConnectionChange(QSet<QObject *> objects)
{
#warning
    return true;
}

bool Backend::connectNodes(QObject *source, QObject *sink)
{
#warning
    return true;
}

bool Backend::disconnectNodes(QObject *source, QObject *sink)
{
#warning
    return true;
}

QStringList Backend::availableMimeTypes() const
{
#warning
    Q_ASSERT(false);
    return QStringList();
}

} // namespace Fake
} // namespace Phonon
