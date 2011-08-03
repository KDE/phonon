/*
    Copyright (c) 2003-2005 Max Howell <max.howell@methylblue.com>
    Copyright (c) 2007-2009 Mark Kretschmann <kretschmann@kde.org>
    Copyright (c) 2010 Kevin Funk <krf@electrostorm.net>
    Copyright (c) 2011 Harald Sitter <sitter@kde.org>
    Copyright (c) 2011 Romain Perier <romain.perier@gmail.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QtCore/QtGlobal>
#if QT_VERSION >= 0x040700
# include <QtCore/QElapsedTimer>
#else
# include <QtCore/QTime>
#endif

#include "davros_p.h"
#include "block.h"

static QString colorize( const QString &text, int color, const QString & area)
{
  if( !Davros::debugColorEnabled(area) )
        return text;
    int colors[] = { 1, 2, 4, 5, 6 }; // no yellow and white for sanity
    return QString( "\x1b[00;3%1m%2\x1b[00;39m" ).arg( QString::number(colors[color]), text );
}

namespace Davros
{

struct BlockPrivate
{
    BlockPrivate(const char *label_, const QString & area_)
      : label(label_)
      , area(area_)
      , color(ContextPrivate::instance(area_)->colorIndex)
    {

    }
#if QT_VERSION >= 0x040700
    QElapsedTimer startTime;
#else
    QTime startTime;
#endif
    const char *label;
    int color;
    QString area;
};

Block::Block(const char *label, const QString & area)
    : d(new BlockPrivate(label, area))
{
    if(!debugEnabled(area)) {
        return;
    }
#if QT_VERSION >= 0x040700
    d->startTime.start();
#else
    d->startTime = QTime::currentTime();
#endif

    ContextPrivate::instance(area)->mutex.lock();
    ContextPrivate::instance(area)->colorIndex = (ContextPrivate::instance(area)->colorIndex + 1) % 5;
    ContextPrivate::instance(area)->mutex.unlock();

    dbgstream(DEBUG_INFO, area)
        << qPrintable( colorize( QLatin1String( "BEGIN:" ), d->color, area) )
        << label;

    ContextPrivate::instance(area)->mutex.lock();
    IndentPrivate::instance(area)->m_string += QLatin1String("  ");
    ContextPrivate::instance(area)->mutex.unlock();
}

Block::~Block()
{
    if( !debugEnabled(d->area)) {
        return;
    }
#if QT_VERSION >= 0x040700
    const double duration = d->startTime.elapsed() / 1000.0;
#else
    const double duration = (double)d->startTime.msecsTo( QTime::currentTime() ) / 1000.0;
#endif

    ContextPrivate::instance(d->area)->mutex.lock();
    IndentPrivate::instance(d->area)->m_string.truncate(IndentPrivate::instance(d->area)->m_string.length() - 2);
    ContextPrivate::instance(d->area)->mutex.unlock();

    // Print timing information, and a special message (DELAY) if the method took longer than 5s
    if( duration < 5.0 ) {
        dbgstream(DEBUG_INFO, d->area)
            << qPrintable(colorize(QLatin1String( "END__:" ), d->color, d->area))
            << d->label
            << qPrintable(colorize(QString( "[Took: %3s]").arg(QString::number(duration, 'g', 2)), d->color, d->area));
    } else {
        dbgstream(DEBUG_INFO, d->area)
            << qPrintable(colorize(QString("END__:"), d->color, d->area))
            << d->label
            << qPrintable(reverseColorize(QString("[DELAY Took (quite long) %3s]")
                                          .arg(QString::number(duration, 'g', 2)), toColor(DEBUG_WARN), d->area));
    }
    delete d;
}

}
