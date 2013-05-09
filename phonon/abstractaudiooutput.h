/*
    Copyright (C) 2005-2006 Matthias Kretz <kretz@kde.org>
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

#ifndef PHONON_ABSTRACTAUDIOOUTPUTBASE_H
#define PHONON_ABSTRACTAUDIOOUTPUTBASE_H

#include "phonondefs.h"
#include "medianode.h"

#include <QtCore/QObject>

namespace Phonon {

class AbstractAudioOutputPrivate;

/** \class AbstractAudioOutput abstractaudiooutput.h phonon/AbstractAudioOutput
 * Common base class for all audio outputs.
 *
 * \see AudioOutput
 */
class PHONON_EXPORT AbstractAudioOutput : public QObject, public MediaNode
{
#warning friending P
    friend class Player;
    Q_OBJECT

public:
    ~AbstractAudioOutput();

protected:
    AbstractAudioOutput(AbstractAudioOutputPrivate &dd, QObject *parent);

private:
    P_DECLARE_PRIVATE(AbstractAudioOutput)
};

} //namespace Phonon

#endif // PHONON_ABSTRACTAUDIOOUTPUTBASE_H
