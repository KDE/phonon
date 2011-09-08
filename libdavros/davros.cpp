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


class LoggerDebugStream: public QIODevice
{
public:
    LoggerDebugStream(const QString & area_) : area(area_) { open(WriteOnly); }
    bool isSequential() const { return false; }
    qint64 readData(char *, qint64) { return 0; /* eof */ }
    qint64 readLineData(char *, qint64) { return 0; /* eof */ }
    qint64 writeData(const char *data, qint64 len);
private:
    QString area;
};

qint64 LoggerDebugStream::writeData(const char *data, qint64 len)
{
    ContextPrivate::instance(area)->logs.localData()->append(data);
    ContextPrivate::instance(area)->logs.localData()->append("\n");
    return len;
}

static inline QDebug nullDebug()
{
    return QDebug(&devnull);
}

IndentPrivate::IndentPrivate(QObject* parent, const QString & area)
    : QObject(parent)
{
    setObjectName( area + QLatin1String("DavrosIndentObject"));
    data.setLocalData(0);
}


IndentPrivate* IndentPrivate::instance(const QString & area)
{
    IndentPrivate *obj;
    QString objectName = area + QLatin1String("DavrosIndentObject");
    if (globalObjectsTable().find(objectName) == globalObjectsTable().end()) {
        obj = new IndentPrivate(qApp, area);
        globalObjectsTable().insert(objectName, reinterpret_cast<void *>(obj));
    }
    obj = reinterpret_cast<IndentPrivate *>(globalObjectsTable()[objectName]);
    if (! obj->data.hasLocalData()) {
        obj->data.setLocalData(new QString(""));
    }
    return obj;
}

ContextPrivate::ContextPrivate(QObject* parent, const QString & area)
    : QObject(parent)
    , mutex(QMutex::Recursive)
{
    setObjectName( area + QLatin1String("DavrosContextObject"));
    debugColorsEnabled = true;
    debugLevel = QtWarningMsg;
    colorIndex = 0;
    loggingType = Davros::Interleaved;
}


ContextPrivate* ContextPrivate::instance(const QString & area)
{
    ContextPrivate *obj;
    QString objectName = area + QLatin1String("DavrosContextObject");
    if (globalObjectsTable().find(objectName) == globalObjectsTable().end()) {
        obj = new ContextPrivate(qApp, area);
        globalObjectsTable().insert(objectName, reinterpret_cast<void *>(obj));
    }
    obj = reinterpret_cast<ContextPrivate *>(globalObjectsTable()[objectName]);
    if (! obj->logs.hasLocalData())
        obj->logs.setLocalData(new QString(""));
    if (! obj->nested.hasLocalData())
        obj->nested.setLocalData(new int(-1));
    return obj;
}

static QString toString(QtMsgType level)
{
    switch(level) {
        case QtWarningMsg:
            return "[WARNING]";
        case QtCriticalMsg:
            return "[ERROR__]";
        case QtFatalMsg:
            return "[FATAL__]";
        default:
            return QString();
    }
}

namespace Davros
{

QString indent(const QString & area)
{
    return *IndentPrivate::instance(area)->data.localData();
}

bool debugColorEnabled(const QString & area)
{
    return ContextPrivate::instance(area)->debugColorsEnabled;
}

QtMsgType minimumDebugLevel(const QString & area)
{
    return ContextPrivate::instance(area)->debugLevel;
}

void setColoredDebug(bool enable, const QString & area)
{
    ContextPrivate::instance(area)->debugColorsEnabled = enable;
}

void setMinimumDebugLevel(QtMsgType level, const QString & area)
{
    ContextPrivate::instance(area)->debugLevel = level;
}

QDebug debugStream(QtMsgType level, const QString & area)
{
    if ( level < minimumDebugLevel(area) )
        return nullDebug();

    QString text = QString("%1%2").arg( area ).arg( *IndentPrivate::instance(area)->data.localData() );
    if ( level > QtDebugMsg )
      text.append( ' ' + reverseColorize( toString(level), toColor( level ), area ) );
    QDebug debug = loggingType(area) == Interleaved ? QDebug(QtDebugMsg) : QDebug(new LoggerDebugStream(area));
    return debug << qPrintable(text);
}

LoggingType loggingType(const QString & area)
{
    return ContextPrivate::instance(area)->loggingType;
}

void setLoggingType(LoggingType type, const QString & area)
{
    ContextPrivate::instance(area)->loggingType = type;
}

}
