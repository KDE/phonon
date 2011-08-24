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

#ifndef PHONON_FAKE_BACKEND_H
#define PHONON_FAKE_BACKEND_H

#include <QtCore/QObject>

#include <phonon/backendinterface.h>

namespace Phonon {
namespace Fake {

class Backend : public QObject, public BackendInterface
{
    Q_OBJECT
public:
    Backend(QObject *parent = 0);
    ~Backend();

    // Backend Interface
    QObject *createObject(Class c, QObject *parent, const QList<QVariant> &args = QList<QVariant>());

    QList<int> objectDescriptionIndexes(ObjectDescriptionType type) const;

    QHash<QByteArray, QVariant> objectDescriptionProperties(ObjectDescriptionType type, int index) const;

    bool startConnectionChange(QSet<QObject *> objects);
    bool   endConnectionChange(QSet<QObject *> objects);
    bool    connectNodes(QObject *source, QObject *sink);
    bool disconnectNodes(QObject *source, QObject *sink);

    QStringList availableMimeTypes() const;
};

} // namespace Fake
} // namespace Phonon

#endif // PHONON_FAKE_BACKEND_H
