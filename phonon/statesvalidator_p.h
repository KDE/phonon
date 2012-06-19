/*
    Copyright (C) 2012 Trever Fischer <tdfischer@fedoraproject.org>
    Copyright (C) 2012 Harald Sitter <sitter@kde.org>

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

#ifndef PHONON_STATESVALIDATOR_P_H
#define PHONON_STATESVALIDATOR_P_H

#include <QtCore/QObject>

#include "phononnamespace.h"

namespace Phonon
{

class MediaObject;

class StatesValidator : public QObject
{
    Q_OBJECT
public:
    explicit StatesValidator(MediaObject *parent = 0);
    ~StatesValidator();

    inline void sourceQueued() { m_sourceQueued = true; }

private slots:
    void validateStateChange(Phonon::State, Phonon::State);
    void validateTick(qint64 tick);
    void validateAboutToFinish();
    void validateFinished();
    void validateBufferStatus();
    void validateSourceChange();

private:
    bool validateStateTransition(Phonon::State newstate, Phonon::State oldstate);

    MediaObject *m_mediaObject;

    Phonon::State m_prevState; /** < Track prev state to do a better job at Buffering validation */

    bool m_sourceQueued; /** < Track whether a source was queued in the backend */
    qint64 m_pos; /** < Track position for abouttofinish validation */

    bool m_aboutToFinishEmitted;
    bool m_aboutToFinishBeforeSeek; /** < True when a seek was conducted before a source change */
    qint64 m_aboutToFinishPos; /** < Position when abouttofinish was emitted */
};

} // namespace Phonon

#endif // PHONON_STATESVALIDATOR_P_H
