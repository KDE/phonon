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

#ifndef PHONON_DECLARATIVE_AUDIOOUTPUTELEMENT_H
#define PHONON_DECLARATIVE_AUDIOOUTPUTELEMENT_H

#include <QtDeclarative/QDeclarativeItem>

#include "abstractinitable.h"

namespace Phonon {

class AudioOutput;
class MediaObject;

namespace Declarative {

/**
 * This is the Qt Quick Element encasing a Phonon::AudioOutput.
 * For general information regarding capabilities please see the documentation
 * of Phonon::AudioOutput.
 *
 * Like every Phonon Qt Quick class this class provides semi-lazy initialization
 * as provided described by the AbstractInitAble class.
 *
 * This element cannot be decorated by another output. If you still try to do
 * so the output will simply attach to the MediaObject this AudioOutputElement
 * was attached to.
 *
 * \see Phonon::AudioOutput
 * \author Harald Sitter <sitter@kde.org>
 */
class AudioOutputElement : public QDeclarativeItem, public AbstractInitAble
{
    Q_OBJECT
    /// \see Phonon::AudioOutput::muted
    Q_PROPERTY(bool muted READ isMuted WRITE setMuted NOTIFY mutedChanged)
    /// \see Phonon::AudioOutput::name
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    /// \see Phonon::AudioOutput::volume
    Q_PROPERTY(qreal volume READ volume WRITE setVolume NOTIFY volumeChanged)
    friend class VolumeFaderEffectElement;
public:
    AudioOutputElement(QDeclarativeItem *parent = 0);
    ~AudioOutputElement();

    /// \reimp
    void classBegin();

    /// \reimp
    virtual void init(MediaObject *mediaObject);

    bool isMuted() const;
    void setMuted(bool muted);

    QString name() const;
    void setName(const QString &name);

    qreal volume() const;
    void setVolume(qreal newVolume);

signals:
    /// emitted when the value of muted changed
    void mutedChanged();

    /// emitted when the value of name changed
    void nameChanged();

    /// emitted when the value of volume changed
    void volumeChanged();

private:
    /// Phonon::AudioOutput exposed by the instance of the AudioOutputElement
    AudioOutput *m_audioOutput;
};

} // namespace Declarative
} // namespace Phonon

#endif // PHONON_DECLARATIVE_AUDIOOUTPUTELEMENT_H
