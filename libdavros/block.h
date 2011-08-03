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

#ifndef DAVROS_BLOCK_H
#define DAVROS_BLOCK_H

#include "davros.h"

/**
 * Convenience macro for making a standard Davros::Block
 */
#define DAVROS_BLOCK Davros::Block uniquelyNamedStackAllocatedStandardBlock( __PRETTY_FUNCTION__, DAVROS_DEBUG_AREA);

namespace Davros
{
struct BlockPrivate;
/**
 * @class Davros::Block
 * @short Use this to label sections of your code
 *
 * Usage:
 *
 *     void function()
 *     {
 *         Debug::Block myBlock( "section" );
 *
 *         debug() << "output1" << endl;
 *         debug() << "output2" << endl;
 *     }
 *
 * Will output:
 *
 *     app: BEGIN: section
 *     app:  [prefix] output1
 *     app:  [prefix] output2
 *     app: END: section - Took 0.1s
 *
 */
class Block
{
public:
    Block( const char *name, const QString & area);
    ~Block();

private:
    BlockPrivate *d;
};
}

#endif
