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

#ifndef DABROS_H
#define DABROS_H

// We always want debug output available at runtime
#undef QT_NO_DEBUG_OUTPUT
#undef KDE_NO_DEBUG_OUTPUT

#include <QtCore/QDebug>
#include <QtCore/QMutex>


// Platform specific macros
#ifdef _WIN32
#define __PRETTY_FUNCTION__ __FUNCTION__
#endif
#ifdef __SUNPRO_CC
#define __PRETTY_FUNCTION__ __FILE__
#endif

#ifndef DAVROS_DEBUG_AREA
# define DAVROS_DEBUG_AREA QLatin1String("(Unknow)")
#endif

/// Standard function announcer
#define DAVROS_FUNC_INFO { Davros::debug() << Davros::indent() << "Func: " << __PRETTY_FUNCTION__;}

/// Announce a line
#define DAVROS_LINE_INFO { Davros::debug() << Davros::indent() << "Line: " << __LINE__; }

/**
 * Convenience macro, use this to remind yourself to finish the implementation of a function
 * The function name will appear in the output
 */
#define DAVROS_NOTIMPLEMENTED warning() << "NOT-IMPLEMENTED:" << __PRETTY_FUNCTION__ << endl;

/**
 * Convenience macro, use this to alert other developers to stop using a function
 * The function name will appear in the output
 */
#define DAVROS_DEPRECATED warning() << "DEPRECATED:" << __PRETTY_FUNCTION__ << endl;

/// Performance logging
#define DAVROS_PERF_LOG( msg ) { Davros::perfLog( msg, __PRETTY_FUNCTION__ ); }

/**
 * @namespace Davros
 * @short kdebug with indentation functionality and convenience macros
 *
 * Usage:
 *
 *     #define DAVROS_DEBUG_AREA "Blah"
 *     #include <davros/davros.h>
 *
 *     void function()
 *     {
 *        Davros::Block myBlock( __PRETTY_FUNCTION__ );
 *
 *        Davros::debug() << "output1" << endl;
 *        Davros::debug() << "output2" << endl;
 *     }
 *
 * Will output:
 *
 * app: BEGIN: void function()
 * app:   [Blah] output1
 * app:   [Blah] output2
 * app: END: void function(): Took 0.1s
 *
 * @see Block
 */
namespace Davros
{

enum DebugLevel {
    DEBUG_INFO  = 0,
    DEBUG_WARN  = 1,
    DEBUG_ERROR = 2,
    DEBUG_FATAL = 3,
    DEBUG_NONE = 4
};

QDebug dbgstream( DebugLevel level = DEBUG_INFO, const QString & area = DAVROS_DEBUG_AREA);
bool debugEnabled(const QString & area = DAVROS_DEBUG_AREA);
bool debugColorEnabled(const QString & area = DAVROS_DEBUG_AREA);
DebugLevel minimumDebugLevel(const QString & area = DAVROS_DEBUG_AREA);
void setColoredDebug( bool enable, const QString & area = DAVROS_DEBUG_AREA);
void setMinimumDebugLevel( DebugLevel level, const QString & area = DAVROS_DEBUG_AREA);
QString indent(const QString & area = DAVROS_DEBUG_AREA);

inline QDebug dbgstreamwrapper( DebugLevel level , const QString & area) { return dbgstream( level , area ); }

inline QDebug debug(const QString & area = DAVROS_DEBUG_AREA)   { return dbgstreamwrapper(DEBUG_INFO, area); }
inline QDebug warning(const QString & area = DAVROS_DEBUG_AREA) { return dbgstreamwrapper(DEBUG_WARN, area); }
inline QDebug error(const QString & area = DAVROS_DEBUG_AREA)   { return dbgstreamwrapper(DEBUG_ERROR, area); }
inline QDebug fatal(const QString & area = DAVROS_DEBUG_AREA)   { return dbgstreamwrapper( DEBUG_FATAL, area ); }


/**
 * @class Debug::List
 * @short You can pass anything to this and it will output it as a list
 *
 *     debug() << (Debug::List() << anInt << aString << aQStringList << aDouble) << endl;
 */
typedef QList<QVariant> List;

}

#endif
