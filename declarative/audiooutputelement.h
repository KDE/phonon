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

#ifndef AUDIOOUTPUTELEMENT_H
#define AUDIOOUTPUTELEMENT_H

#include <QtDeclarative/QDeclarativeItem>

#include <phonon/mediaobject.h>

#include "abstractinitable.h"

namespace Phonon {

class AudioOutput;

namespace Declarative {

class AudioOutputElement : public QDeclarativeItem, public AbstractInitAble
{
    Q_OBJECT
    Q_PROPERTY(bool muted READ isMuted WRITE setMuted NOTIFY mutedChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(qreal volume READ volume WRITE setVolume NOTIFY volumeChanged)
public:
    AudioOutputElement(QDeclarativeItem *parent = 0);
    ~AudioOutputElement();

    bool isMuted() const;
    void setMuted(bool muted);

    QString name() const;
    void setName(const QString &name);

    qreal volume() const;
    void setVolume(qreal newVolume);

    virtual void init(MediaObject *mediaObject);

signals:
    void mutedChanged();
    void nameChanged();
    void volumeChanged();

private:
    void processPendingProperties();

    QHash<QByteArray, QVariant> m_pendingProperties;

    AudioOutput *m_audioOutput;
};

} // namespace Declarative
} // namespace Phonon

#endif // AUDIOOUTPUTELEMENT_H
