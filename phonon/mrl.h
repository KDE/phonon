/*  This file is part of the KDE project
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

#ifndef PHONON_MRL_H
#define PHONON_MRL_H

#include <QtCore/QUrl>

#include "phonon_export.h"


namespace Phonon
{

/** \class Mrl mrl.h phonon/Mrl
 * Media Resource Locator - A QUrl particularly for MediaSources.
 *
 * Whenever working with URLs/URIs that actually are referencing a media resource
 * a Mrl should be used rather than a QUrl. Mrl has functions particularly
 * made for a media related use case and additionally overloads some of QUrl's
 * functions to work more reasonable in a Phonon context.
 *
 * A Mrl can be used just like a QUrl.
 *
 * \ingroup Playback
 * \author Harald Sitter <sitter@kde.org>
 */
class PHONON_EXPORT Mrl : public QUrl
{
public:
    Mrl();
    Mrl(const Mrl &copy);

    Mrl(const QUrl &url);
    Mrl(const QString &url);

    Mrl &operator =(const Mrl &copy);
#ifndef QT_NO_URL_CAST_FROM_STRING
    Mrl &operator =(const QString &url);
#endif

    /**
     * Returns the encoded representation of the MRL if it's valid; otherwise
     * an empty QByteArray is returned. The output can be customized by passing
     * flags with options.
     *
     * If the MRL is a local file it will first be encoded using the local encoding,
     * otherwise it gets converted to UTF-8, and all non-ASCII characters are
     * then percent encoded.
     *
     * \arg options QUrl::FormattingOptions to manipulate what the output should look like
     *
     * \returns the encoded MRL or an empty QByteArray if the MRL was invalid
     */
    QByteArray toEncoded(FormattingOptions options = None) const;
};

} // namespace Phonon


#endif // PHONON_MRL_H
