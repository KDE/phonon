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

#include "davros.h"
#include "davros_p.h"

#include <QtCore/QMutex>
#include <QtCore/QObject>
#include <QtGui/QApplication>

#ifdef Q_OS_UNIX
# include <unistd.h>
#endif

// Avoids the "static initialization order fiasco" problem
// See http://www.parashift.com/c++-faq-lite/ctors.html#faq-10.14
typedef QMap<QString, void *> GlobalContextTable;

static GlobalContextTable& globalObjectsTable()
{
    static GlobalContextTable* instance = new GlobalContextTable;
    return *instance;
}

class NoDebugStream: public QIODevice
{
    // Q_OBJECT
public:
    NoDebugStream() { open(WriteOnly); }
    bool isSequential() const { return true; }
    qint64 readData(char *, qint64) { return 0; /* eof */ }
    qint64 readLineData(char *, qint64) { return 0; /* eof */ }
    qint64 writeData(const char *, qint64 len) { return len; }
} devnull;

static inline QDebug nullDebug()
{
    return QDebug(&devnull);
}

IndentPrivate::IndentPrivate(QObject* parent, const QString & area)
    : QObject(parent)
{
    setObjectName( area + QLatin1String("DavrosIndentObject"));
}


IndentPrivate* IndentPrivate::instance(const QString & area)
{
    IndentPrivate *obj;
    QString objectName = area + QLatin1String("DavrosIndentObject");
    if (globalObjectsTable().find(objectName) == globalObjectsTable().end()) {
        obj = new IndentPrivate(qApp, area);
        globalObjectsTable().insert(objectName, reinterpret_cast<void *>(obj));
	return obj;
    }
    obj = reinterpret_cast<IndentPrivate *>(globalObjectsTable()[objectName]);
    return obj;
}

ContextPrivate::ContextPrivate(QObject* parent, const QString & area)
    : QObject(parent)
    , mutex(QMutex::Recursive)
{
    setObjectName( area + QLatin1String("DavrosContextObject"));
    debugColorsEnabled = true;
    debugLevel = Davros::DEBUG_WARN;
    colorIndex = 0;
}


ContextPrivate* ContextPrivate::instance(const QString & area)
{
    ContextPrivate *obj;
    QString objectName = area + QLatin1String("DavrosContextObject");
    if (globalObjectsTable().find(objectName) == globalObjectsTable().end()) {
        obj = new ContextPrivate(qApp, area);
        globalObjectsTable().insert(objectName, reinterpret_cast<void *>(obj));
	return obj;
    }
    obj = reinterpret_cast<ContextPrivate *>(globalObjectsTable()[objectName]);
    return obj;
}

static QString toString( Davros::DebugLevel level )
{
    switch( level )
    {
    case Davros::DEBUG_WARN:
            return "[WARNING]";
    case Davros::DEBUG_ERROR:
            return "[ERROR__]";
    case Davros::DEBUG_FATAL:
            return "[FATAL__]";
        default:
            return QString();
    }
}

namespace Davros
{

QString indent(const QString & area)
{
    return IndentPrivate::instance(area)->m_string;
}

bool debugEnabled(const QString & area)
{
    return ContextPrivate::instance(area)->debugLevel < DEBUG_NONE;
}

bool debugColorEnabled(const QString & area)
{
    return ContextPrivate::instance(area)->debugColorsEnabled;
}

DebugLevel minimumDebugLevel(const QString & area)
{
    return ContextPrivate::instance(area)->debugLevel;
}

void setColoredDebug(bool enable, const QString & area)
{
    ContextPrivate::instance(area)->debugColorsEnabled = enable;
}

void setMinimumDebugLevel(DebugLevel level, const QString & area)
{
    ContextPrivate::instance(area)->debugLevel = level;
}

QDebug dbgstream( DebugLevel level, const QString & area)
{
  if ( level < minimumDebugLevel(area) )
        return nullDebug();

    ContextPrivate::instance(area)->mutex.lock();
    const QString currentIndent = IndentPrivate::instance(area)->m_string;
    ContextPrivate::instance(area)->mutex.unlock();

    QString text = QString("%1%2").arg( area ).arg( currentIndent );
    if ( level > DEBUG_INFO )
      text.append( ' ' + reverseColorize( toString(level), toColor( level ), area ) );

    return QDebug( QtDebugMsg ) << qPrintable( text );
}

}
