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

#ifndef PHONON_KEEPREFERENCE_H
#define PHONON_KEEPREFERENCE_H

#include <QtCore/QExplicitlySharedDataPointer>
#include <QtCore/QObject>
#include <QtGui/QApplication>
#include "shareddata.h"
#include "xinethread.h"

namespace Phonon
{
namespace Xine
{

template<int TIME = 10000>
class KeepReference : public QObject
{
    public:
        inline KeepReference()
        {
            //moveToThread(QApplication::instance()->thread());
            moveToThread(XineThread::instance());
            Backend::addCleanupObject(this);
        }

        inline ~KeepReference() { Backend::removeCleanupObject(this); }

        inline void addObject(SharedData *o) { objects << QExplicitlySharedDataPointer<SharedData>(o); }
        inline void ready() {
            // do this so that startTimer is called from the correct thread
            QCoreApplication::postEvent(this, new QEvent(static_cast<QEvent::Type>(2345)));
        }

    protected:
        bool event(QEvent *e)
        {
            if (e->type() == 2345) {
                e->accept();
                startTimer(TIME);
                return true;
            }
            return QObject::event(e);
        }

        void timerEvent(QTimerEvent *e)
        {
            killTimer(e->timerId());
            deleteLater();
        }

    private:
        QList<QExplicitlySharedDataPointer<SharedData> > objects;
};


} // namespace Xine
} // namespace Phonon
#endif // PHONON_KEEPREFERENCE_H
