/*
    Copyright (C) 2005 Matthias Kretz <kretz@kde.org>
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

#ifndef PHONON_PLAYER_H
#define PHONON_PLAYER_H

#include "frontend.h"
#include "source.h"

namespace Phonon {

class AbstractAudioOutput;
class AbstractVideoOutput;
class PlayerPrivate;

class PHONON_EXPORT Player : public QObject, public Frontend
{
    Q_OBJECT
public:
    Player(QObject *parent = 0);
    ~Player();

    State state() const;
    bool isSeekable() const;
    qint32 tickInterval() const;
    QStringList metaData(const QString &key) const;
    QStringList metaData(Phonon::MetaData key) const;
    QMultiMap<QString, QString> metaData() const;
    QString errorString() const;
    ErrorType errorType() const;
    Source source() const;
    void setSource(const Source &source);
    qint64 currentTime() const;
    qint64 totalTime() const;
    qint64 remainingTime() const;

#warning merge outputs?
    void addAudioOutput(AbstractAudioOutput *audioOutput);
    void addVideoOutput(AbstractVideoOutput *videoOutput);

public Q_SLOTS:
    void setTickInterval(qint32 newTickInterval);
    void play();
    void pause();
    void stop();
    void seek(qint64 time);

Q_SIGNALS:
    void stateChanged(Phonon::State newstate, Phonon::State oldstate);
    void tick(qint64 time);
    void metaDataChanged();
    void seekableChanged(bool isSeekable);
    void bufferStatus(int percentFilled);
    void finished();
    void currentSourceChanged(const Source &newSource);
    void totalTimeChanged(qint64 newTotalTime);

private:
    P_DECLARE_PRIVATE(Player)
    Q_PRIVATE_SLOT(k_func(), void _k_resumePlay())
    Q_PRIVATE_SLOT(k_func(), void _k_resumePause())
    Q_PRIVATE_SLOT(k_func(), void _k_metaDataChanged(const QMultiMap<QString, QString> &))
};

} //namespace Phonon

#endif // PHONON_PLAYER_H
