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

#ifndef DAVROS_H
#define DAVROS_H

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
# define DAVROS_DEBUG_AREA QLatin1String("(Unknown)")
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


/**
 * @namespace Davros
 * @short kdebug with indentation functionality and convenience macros
 *
 * Usage:
 * <code>
 *     #define DAVROS_DEBUG_AREA "Blah"
 *     #include <davros/davros.h>
 *     #include <davros/block.h>
 *
 *     void function()
 *     {
 *         DAVROS_BLOCK;
 *         Davros::debug() << "debug1";
 *         Davros::debug() << "debug2";
 *         nested_function();
 *     }
 *
 *     void nested_function()
 *     {
 *         DAVROS_BLOCK;
 *         Davros::debug() << "debug3";
 *         Davros::debug() << "debug4";
 *     }
 * </code>
 *
 * Will output:
 *
 * Blah BEGIN: void function() [Thread 1]
 * Blah  [Thread 1] debug1
 * Blah  [Thread 1] debug2
 * Blah  BEGIN: void nested_function() [Thread 1]
 * Blah    [Thread 1] debug3
 * Blah    [Thread 1] debug4
 * Blah  END__: void nested_function() [Thread 1]
 * Blah END__: void function(): [Thread 1] Took 0.1s
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

/**
 * @internal
 * Returns a debug stream that may or may not output anything.
 */
QDebug dbgstream(DebugLevel level = DEBUG_INFO, const QString & area = DAVROS_DEBUG_AREA);

/**
 * @internal
 * Returns a string representing the current indentation used for nested blocks
 */
QString indent(const QString & area = DAVROS_DEBUG_AREA);

bool debugEnabled(const QString & area = DAVROS_DEBUG_AREA);

/**
 * Returns a boolean indicating if outputs are colorized
 * @param area an id to identify the output, DAVROS_DEBUG_AREA for default
 * \see setColoredDebug
 */
bool debugColorEnabled(const QString & area = DAVROS_DEBUG_AREA);

/**
 * Returns the current debugging level
 *
 * All kind of messages with a level greater or equal to this minimul level
 * Will be displayed.
 * @param area an id to identify the output, DAVROS_DEBUG_AREA for default
 * \see setMinimumDebugLevel
 */
DebugLevel minimumDebugLevel(const QString & area = DAVROS_DEBUG_AREA);

/**
 * Sets/Unsets colorized outputs
 * @param enable a boolean used to enable/disable colors
 * @param area an id to identify the output, DAVROS_DEBUG_AREA for default
 * \see debugColorEnabled
 */
void setColoredDebug( bool enable, const QString & area = DAVROS_DEBUG_AREA);

/**
 * Sets the current debugging level
 *
 * All kind of messages with a level greater or equal to this minimul level
 * Will be displayed.
 * By default, all warnings, errors or fatals are logged and the debugging
 * level is DEBUG_WARN.
 * @param level the debugging level to use
 * @param area an id to identify the output, DAVROS_DEBUG_AREA for default
 * \see minimumDebugLevel
 */
void setMinimumDebugLevel( DebugLevel level, const QString & area = DAVROS_DEBUG_AREA);

/**
 * Returns a debug stream. You can use it to print debug
 * information.
 * @param area an id to identify the output, DAVROS_DEBUG_AREA for default
 */
static inline QDebug debug(const QString & area = DAVROS_DEBUG_AREA)
{ return dbgstream(DEBUG_INFO, area); }

/**
 * Returns a warning stream. You can use it to print warning
 * information.
 * @param area an id to identify the output, DAVROS_DEBUG_AREA for default
 */
static inline QDebug warning(const QString & area = DAVROS_DEBUG_AREA)
{ return dbgstream(DEBUG_WARN, area); }

/**
 * Returns an error stream. You can use it to print error
 * information.
 * @param area an id to identify the output, DAVROS_DEBUG_AREA for default
 */
static inline QDebug error(const QString & area = DAVROS_DEBUG_AREA)
{ return dbgstream(DEBUG_ERROR, area); }

/**
 * Returns a fatal error stream. You can use it to print fatal error
 * information.
 * @param area an id to identify the output, DAVROS_DEBUG_AREA for default
 */
static inline QDebug fatal(const QString & area = DAVROS_DEBUG_AREA)
{ return dbgstream(DEBUG_FATAL, area ); }

}

#endif
