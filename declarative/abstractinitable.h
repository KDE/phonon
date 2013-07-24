/*
    Copyright (C) 2011 Harald Sitter <sitter@kde.org>

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

#ifndef PHONON_DECLARATIVE_ABSTRACTINITABLE_H
#define PHONON_DECLARATIVE_ABSTRACTINITABLE_H

#include <QtCore/QObject>

#include "phonon/path.h"

namespace Phonon {

class MediaObject;

namespace Declarative {

/**
 * Helps with lazy init protection.
 * It checks whether a specified member variable evaluates to false (null pointer,
 * false bool...) and if it does returns a default initialized instance of a
 * specified return type.
 *
 * \param member the member variable to check
 * \param ret the return type to return on failure
 */
#define P_D_PROTECT(member, ret) if (!member) return ret()

/**
 * Helps with lazy init protection.
 * It checks whether a specified member variable evaluates to false (null pointer,
 * false bool...) and if it does returns specified return value.
 *
 * \param member the member variable to check
 * \param ret the return value to return on failure
 */
#define P_D_PROTECT_VALUE(member, ret) if (!member) return ret

/**
 * This class allows the initialization of QObjects in an object tree.
 * All Phonon declarative items are derived from an AbstractInitAble and something
 * based on QObject (one way or another). They implement the init function which
 * usually receives a MediaObject instance as argument. Mostly a MediaElement will
 * kick off the init chain by calling initChildren() whcih consequently calls
 * init() on all children that are AbstractInitAble.
 *
 * This enables implicit Phonon Path creation by implicit nesting:
 * \code
 * Media {
 *     Audio {}
 *     Video { Subtitle {} }
 * }
 * \endcode
 *
 * Which roughly translates to a call chain like this:
 * \verbatim
 * MediaElement::componentComplete()
 *  MediaElement::initChildren()
 *   AudioElement::init()
 *   VideoElement::init()
 *    VideoElement::initChildren()
 *     SubtitleElement::init()
 * \endverbatim
 *
 * Within each init() the element creates a patch between the MO and its own
 * Phonon MediaNode. In a decorator element such as the subtitle it will get its
 * parentItem and try to obtain the appropriate Phonon MediaNode from it (if
 * necessary at all).
 *
 * The explicit check of the parent is necessary to ensure that the nesting
 * represents a valid path and also since most decorators will not
 * actually decorate its parent but in fact the MediaObject.
 * This is particularly true since most Video decorators are actually bundled in
 * the Phonon::MediaController class which attaches to a MediaObject.
 *
 * \author Harald Sitter <sitter@kde.org>
 */
class AbstractInitAble
{
public:
    /**
     * Called by the parent item upon init. This can be either once the
     * component is complete or at any given point later in the life time of the
     * component.
     *
     * There are two options to handle init. One is to create the underlying
     * Phonon MediaNode right after creation of your instance or at a declarative
     * parser status update. The other is to only do it once init() is called.
     *
     * While the latter option permits startup improvements due to lazy init()
     * up until the point when the user actually tries to use the Phonon graph
     * it also requires you to protect your class from accesses to uninitialized
     * members before init() was called.
     *
     * To help with the protection there are the macros P_D_PROTECT and
     * P_D_PROTECT_VALUE that help with protecting a member by either returning
     * a default constructed object or a given type.
     */
    virtual void init(MediaObject *mediaObject = 0) = 0;

protected:
    AbstractInitAble() : m_mediaObject(0) {}
    virtual ~AbstractInitAble() {}

    /**
     * Traverse child objects and try to cast them to AbstractInitAble. On success
     * call init with the own m_mediaObject.
     */
    inline void initChildren(QObject *that) const
    {
        foreach (QObject *qobject, that->children()) {
            AbstractInitAble *obj = dynamic_cast<AbstractInitAble *>(qobject);
            if (obj)
                obj->init(m_mediaObject);
        }
    }

    /** The Phonon::MediaObject that forms the root of the media graph this initable belongs to */
    // Parented by MediaElement.
    MediaObject *m_mediaObject;

    /// The Phonon::Path between Phonon::MediaObject and m_audioOutput
    Path m_path;
};

} // namespace Declarative
} // namespace Phonon

#endif // PHONON_DECLARATIVE_ABSTRACTINITABLE_H
