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

#ifndef PHONON_PLAYERINTERFACE_H
#define PHONON_PLAYERINTERFACE_H

#include "phononnamespace.h"
#include "source.h"

namespace Phonon {

class PlayerInterface
{
public:
    virtual ~PlayerInterface() {}
    virtual void play() = 0;
    virtual void pause() = 0;
    virtual void stop() = 0;
    virtual void seek(qint64 milliseconds) = 0;
    virtual void setTickInterval(qint32 interval) = 0;
    virtual bool isSeekable() const = 0;
    virtual Phonon::State state() const = 0;
    virtual QString errorString() const = 0;
    virtual Phonon::ErrorType errorType() const = 0;
    virtual Source source() const = 0;
    virtual void setSource(const Source &) = 0;

    // Output linking
    virtual void addOutput(QObject *output) = 0;

    // Time
    virtual qint32 tickInterval() const = 0;
    virtual qint64 time() const = 0;
    virtual qint64 totalTime() const = 0;
#warning.... in the interface... really Oo
    virtual qint64 remainingTime() const { return totalTime() - time(); }

Q_SIGNALS:
    virtual void metaDataChanged(QMultiMap<MetaData, QString>) = 0;
    virtual void stateChanged(Phonon::State newState, Phonon::State oldState) = 0;
    virtual void seekableChanged(bool seekable) = 0;
    virtual void bufferStatus(int percent) = 0;
    virtual void currentSourceChanged(const Source &source) = 0;

    // Time
    virtual void onTimeChanged(qint64 time) = 0;
    virtual void totalTimeChanged(qint64 totalTime) = 0;
};

} // namespace Phonon

Q_DECLARE_INTERFACE(Phonon::PlayerInterface, "org.kde.phonon.PlayerInterface/5.0")

#endif // PHONON_PLAYERINTERFACE_H
