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

#ifndef PHONON_PLAYER_P_H
#define PHONON_PLAYER_P_H

#include "frontend_p.h"
#include "phonondefs_p.h"
#include "player.h"
#include "playerinterface.h"

#include <QtCore/QMultiMap>
#include <QtCore/QString>

namespace Phonon {

class FrontendInterfacePrivate;

class PlayerPrivate : public FrontendPrivate
{
    friend class AbstractMediaStream;
    friend class AbstractMediaStreamPrivate;

protected:
    /** \reimp */
    virtual void createBackendObject() Q_DECL_OVERRIDE Q_DECL_FINAL;

#warning this really needs to become virtual in frontendprivate
    void setupBackendObject();

    void _k_resumePlay();
    void _k_resumePause();
    void _k_metaDataChanged(const QMultiMap<MetaData, QString> &);
    void _k_aboutToFinish();
    void _k_currentSourceChanged(const Source &);
    PHONON_EXPORT void _k_stateChanged(Phonon::State, Phonon::State);
    void streamError(Phonon::ErrorType, const QString &);

    PlayerPrivate()
        : tickInterval(0)
        , metaData()
        , state(Phonon::StoppedState)
        , abstractStream(0)
        , playingQueuedSource(false)
        , errorType(Phonon::NormalError)
        , interface(0)
    {}

    ~PlayerPrivate() {}

#warning base uses m_ prefix, playerprivate does not....
    qint32 tickInterval;
    QMultiMap<MetaData, QString> metaData;
    State state;
    AbstractMediaStream *abstractStream;
    bool playingQueuedSource;
    ErrorType errorType;
    Source source;

    QList<AbstractOutput *> outputs;

    PlayerInterface *interface;

private:
    P_DECLARE_PUBLIC(Player)
};

} // namespace Phonon

#endif // PHONON_PLAYER_P_H
