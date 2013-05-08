/*  This file is part of the KDE project
    Copyright (C) 2007 Matthias Kretz <kretz@kde.org>

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

#ifndef PHONON_PATH_H
#define PHONON_PATH_H

#include "phonon_export.h"
#include "objectdescription.h"

#include <QtCore/QExplicitlySharedDataPointer>


template<class T> class QList;

namespace Phonon
{

class PathPrivate;
class MediaNode;

/** \class Path path.h phonon/Path
 * \short Connection object providing convenient effect insertion
 *
 * \code
MediaObject *media = new MediaObject;
AudioOutput *output = new AudioOutput(Phonon::MusicCategory);
Path path = Phonon::createPath(media, output);
Q_ASSERT(path.isValid()); // for this simple case the path should always be
                          //valid - there are unit tests to ensure it
// insert an effect
QList<EffectDescription> effectList = BackendCapabilities::availableAudioEffects();
if (!effectList.isEmpty()) {
    Effect *effect = path.insertEffect(effectList.first());
}
 * \endcode
 * \ingroup Playback
 * \ingroup Recording
 * \author Matthias Kretz <kretz@kde.org>
 * \author Thierry Bastian <thierry.bastian@trolltech.com>
 */
class PHONON_EXPORT Path
{
    friend class FactoryPrivate;
    public:
        /**
         * Destroys this reference to the Path. If the path was valid the connection is not broken
         * as both the source and the sink MediaNodes still keep a reference to the Path.
         *
         * \see disconnect
         */
        ~Path();

        /**
         * Creates an invalid path.
         *
         * You can still make it a valid path by calling reconnect. To create a path you should use
         * createPath, though.
         *
         * \see createPath
         * \see isValid
         */
        Path();

        /**
         * Constructs a copy of the given path.
         *
         * This constructor is fast thanks to explicit sharing.
         */
        Path(const Path &);

        /**
         * Returns whether the path object connects two MediaNodes or not.
         *
         * \return \p true when the path connects two MediaNodes
         * \return \p false when the path is disconnected
         */
        bool isValid() const;
        //MediaStreamTypes mediaStreamTypes() const;


        /**
         * Tries to change the MediaNodes the path is connected to.
         *
         * If reconnect fails the old connection is kept.
         */
        bool reconnect(MediaNode *source, MediaNode *sink);

        /**
         * Disconnects the path from the MediaNodes it was connected to. This invalidates the path
         * (isValid returns \p false then).
         */
        bool disconnect();

        /**
         * Assigns \p p to this Path and returns a reference to this Path.
         *
         * This operation is fast thanks to explicit sharing.
         */
        Path &operator=(const Path &p);

        /**
         * Returns \p true if this Path is equal to \p p; otherwise returns \p false;
         */
        bool operator==(const Path &p) const;

        /**
         * Returns \p true if this Path is not equal to \p p; otherwise returns \p false;
         */
        bool operator!=(const Path &p) const;

        /**
         * Returns the source MediaNode used by the path.
         */
        MediaNode *source() const;

        /**
         * Returns the sink MediaNode used by the path.
         */
        MediaNode *sink() const;


    protected:
        friend class PathPrivate;
        QExplicitlySharedDataPointer<PathPrivate> d;
};

/**
 * \relates Path
 * Creates a new Path connecting two MediaNodes.
 *
 * The implementation will automatically select the right format and media type. E.g. connecting a
 * MediaObject and AudioOutput will create a Path object connecting the audio. This might be
 * represented as PCM or perhaps even AC3 depending on the AudioOutput object.
 *
 * \param source The MediaNode to connect an output from
 * \param sink The MediaNode to connect to.
 */
PHONON_EXPORT Path createPath(MediaNode *source, MediaNode *sink);

} // namespace Phonon


#endif // PHONON_PATH_H
