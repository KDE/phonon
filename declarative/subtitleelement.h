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

#ifndef PHONON_DECLARATIVE_SUBTITLEELEMENT_H
#define PHONON_DECLARATIVE_SUBTITLEELEMENT_H

#include <QtCore/QObject>
#include <QtCore/QStringList>

#include "abstractinitable.h"

namespace Phonon {

class MediaController;

namespace Declarative {

/**
 * This is the Qt Quick Element encasing a Phonon::MediaController.
 * For general information regarding capabilities please see the documentation
 * of Phonon::MediaController, but mind that this class is limited to subtitle
 * related actions.
 *
 * Like every Phonon Qt Quick class this class provides semi-lazy initialization
 * as provided described by the AbstractInitAble class.
 *
 * \see Phonon::MediaController
 * \author Harald Sitter <sitter@kde.org>
 */
class SubtitleElement : public QObject, public AbstractInitAble
{
    Q_OBJECT
    Q_PROPERTY(QStringList availableSubtitles READ availableSubtitles NOTIFY availableSubtitlesChanged)
    Q_PROPERTY(QString subtitle READ subtitle WRITE setSubtitle NOTIFY subtitleChanged)
public:
    SubtitleElement(QObject *parent = 0);

    /// \reimp
    virtual void init(MediaObject *mediaObject);

    /// \see Phonon::MediaController::availableSubtitles
    QStringList availableSubtitles() const;

    /// \see Phonon::MediaController::currentSubtitle
    QString subtitle() const;

    /// \see Phonon::MediaController::setCurrentSubtitle
    void setSubtitle(const QString &subtitle);

signals:
    /// \see Phonon::MediaController::availableSubtitilesChanged
    void availableSubtitlesChanged();

    /// emitted when the current subtitle was changed
    void subtitleChanged();

private:
    /**
     * Checks whether the parent item is valid for this element.
     * \returns \c true when parent is either an VideoOutputElement or a
     * MediaElement, \c false otherwise.
     */
    bool isParentValid() const;

    /// The Phonon::MediaController contained by this element.
    MediaController *m_mediaController;
};

} // namespace Declarative
} // namespace Phonon

#endif // PHONON_DECLARATIVE_SUBTITLEELEMENT_H
