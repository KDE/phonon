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

#ifndef PHONON_DECLARATIVE_VIDEOOUTPUTELEMENT_H
#define PHONON_DECLARATIVE_VIDEOOUTPUTELEMENT_H

#include <QtDeclarative/QDeclarativeItem>
#include <QtDeclarative/QDeclarativeParserStatus>

#include "abstractinitable.h"
#include "videoformatspy.h"

namespace Phonon {

class VideoGraphicsObject;

namespace Declarative {

/**
 * This is the Qt Quick Element encasing a Phonon::VideoGraphicsObject.
 * For general information regarding capabilities please see the documentation
 * of Phonon::VideoGraphicsObject.
 *
 * Like every Phonon Qt Quick class this class provides semi-lazy initialization
 * as provided described by the AbstractInitAble class.
 *
 * This element cannot be decorated by another output. If you still try to do
 * so the output will simply attach to the MediaObject this VideoOutputElement
 * was attached to.
 *
 * \see Phonon::VideoGraphicsObject
 * \author Harald Sitter <sitter@kde.org>
 */
class VideoOutputElement : public QDeclarativeItem, public AbstractInitAble
{
    Q_OBJECT
    Q_INTERFACES(QDeclarativeParserStatus)
    Q_PROPERTY(bool cursorVisible READ isCursorVisible WRITE setCursorVisible NOTIFY cursorVisibilityChanged)
    Q_PROPERTY(bool fullScreen READ isFullScreen WRITE setFullScreen NOTIFY fullScreenChanged)
public:
    VideoOutputElement(QDeclarativeItem *parent = 0);
    ~VideoOutputElement();

    /// \reimp
    void classBegin();

    /// \reimp
    virtual void init(MediaObject *mediaObject);

    bool isCursorVisible() const;
    void setCursorVisible(bool visible);

    bool isFullScreen() const;
    void setFullScreen(bool fullScreen);

    void setSpy(VideoFormatSpyElement *spy);

signals:
    void cursorVisibilityChanged();
    void fullScreenChanged();
    void spyChanged();

protected:
    /// Forwards geometry changes to the internal VideoGraphicsObject.
    /// \reimp
    virtual void geometryChanged(const QRectF &newGeometry,
                                 const QRectF &oldGeometry);

private:
    /// The contained VideoGraphicsObject (it is a childitem of this item actually).
    VideoGraphicsObject *m_graphicsObject;

    /// Whether the element is in fullscreen.
    bool m_isFullScreen;

    /*** Spy or 0 */
    VideoFormatSpyElement *m_spy;
};

} // namespace Declarative
} // namespace Phonon

#endif // PHONON_DECLARATIVE_VIDEOOUTPUTELEMENT_H
