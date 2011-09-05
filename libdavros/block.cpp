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
#include <QtCore/QThread>
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
      , color(0)
    {
        ContextPrivate *ctx = ContextPrivate::instance(area_);
        ctx->mutex.lock();
        color = ctx->colorIndex;
        ctx->colorIndex = (ctx->colorIndex + 1) % 5;
        ctx->mutex.unlock();
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
    static int count = 1;

    if(!debugEnabled(area)) {
        return;
    }
#if QT_VERSION >= 0x040700
    d->startTime.start();
#else
    d->startTime = QTime::currentTime();
#endif
    ContextPrivate *ctx = ContextPrivate::instance(area);

    if (QThread::currentThread()->objectName().isEmpty()) {
        ctx->mutex.lock();
        int id = count++;
        ctx->mutex.unlock();
        QThread::currentThread()->setObjectName("Thread " + QString::number(id));
    }

    dbgstream(DEBUG_INFO, area)
        << qPrintable( colorize( QLatin1String( "BEGIN:" ), d->color, area) )
        << label << qPrintable(colorize( "[" + QThread::currentThread()->objectName() + "]", d->color, area));

    IndentPrivate::instance(area)->data.localData()->append(QLatin1String("  "));
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
    IndentPrivate::instance(d->area)->data.localData()->truncate(IndentPrivate::instance(d->area)->data.localData()->length() - 2);

    // Print timing information, and a special message (DELAY) if the method took longer than 5s
    if( duration < 5.0 ) {
        dbgstream(DEBUG_INFO, d->area)
            << qPrintable(colorize(QLatin1String( "END__:" ), d->color, d->area))
            << d->label << qPrintable(colorize("[" + QThread::currentThread()->objectName() + "]", d->color, d->area))
            << qPrintable(colorize(QString( "[Took: %3s]").arg(QString::number(duration, 'g', 2)), d->color, d->area));
    } else {
        dbgstream(DEBUG_INFO, d->area)
            << qPrintable(colorize(QString("END__:"), d->color, d->area))
            << d->label << qPrintable(colorize("[" + QThread::currentThread()->objectName() + "]", d->color, d->area))
            << qPrintable(reverseColorize(QString("[DELAY Took (quite long) %3s]")
                                          .arg(QString::number(duration, 'g', 2)), toColor(DEBUG_WARN), d->area));
    }
    delete d;
}

}
