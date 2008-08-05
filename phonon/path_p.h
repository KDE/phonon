/*  This file is part of the KDE project
    Copyright (C) 2007 Matthias Kretz <kretz@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License version 2 as published by the Free Software Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.

*/

#ifndef PATH_P_H
#define PATH_P_H

#include "path.h"
#include <QtCore/QPair>
#include <QtCore/QList>
#include <QtCore/QSharedData>
#include "effect.h"
#include "medianodedestructionhandler_p.h"

QT_BEGIN_NAMESPACE

class QObject;

namespace Phonon
{

class MediaNode;
typedef QPair<QObject*, QObject*> QObjectPair;


class PathPrivate : public QSharedData, private MediaNodeDestructionHandler
{
    friend class Path;
    public:
        PathPrivate()
            : sourceNode(0), sinkNode(0)
#ifndef QT_NO_PHONON_EFFECT
            , effectsParent(0)
#endif //QT_NO_PHONON_EFFECT
        {
        }

        ~PathPrivate();

        MediaNode *sourceNode;
        MediaNode *sinkNode;

    protected:
        void phononObjectDestroyed(MediaNodePrivate *);

#ifndef QT_NO_PHONON_EFFECT
        QObject *effectsParent; // used as parent for Effects created in insertEffect
        QList<Effect *> effects;
#endif
    private:
        bool executeTransaction( const QList<QObjectPair> &disconnections, const QList<QObjectPair> &connections);
#ifndef QT_NO_PHONON_EFFECT
        bool removeEffect(Effect *effect);
#endif
};

} // namespace Phonon

QT_END_NAMESPACE

#endif // PATH_P_H
