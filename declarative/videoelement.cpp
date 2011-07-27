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

#include "videoelement.h"

#include <QtGui/QApplication>
#include <QtGui/QPainter>

#include <phonon/audiooutput.h>
#include <phonon/mediaobject.h>
#include <phonon/videographicsobject.h>

#include <phonon/experimental/videodataoutput2.h>

using namespace Phonon::Experimental;

namespace Phonon {
namespace Declarative {

#warning there might be awful problems when not deriving from qdeclarativeitem!!! (virtual inheritance?)

VideoElement::VideoElement(QDeclarativeItem *parent) :
    QDeclarativeItem(parent),
    m_graphicsObject(0),
    m_isFullScreen(false)
{
}

VideoElement::~VideoElement()
{
}

void VideoElement::classBegin()
{
    m_graphicsObject = new VideoGraphicsObject(this);
}

void VideoElement::init(MediaObject *mediaObject)
{
#warning inheritance bad -> inits all the phonon without being used
    Q_ASSERT(mediaObject);

    m_mediaObject = mediaObject;
    createPath(m_mediaObject, m_graphicsObject);
}

bool VideoElement::isCursorVisible() const
{
    return cursor().shape() == Qt::BlankCursor ? true : false;
}

void VideoElement::setCursorVisible(bool visible)
{
    setCursor(visible ? QCursor() : QCursor(Qt::BlankCursor));
}

bool VideoElement::isFullScreen() const
{
    return m_isFullScreen;
}

void VideoElement::setFullScreen(bool fullScreen)
{
    if (fullScreen) {
        m_isFullScreen = true;
        qApp->activeWindow()->showFullScreen();
    } else {
        m_isFullScreen = false;
        qApp->activeWindow()->showNormal();
    }
}

void VideoElement::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    m_graphicsObject->setGeometry(newGeometry);
    QDeclarativeItem::geometryChanged(newGeometry, oldGeometry);
}

} // namespace Declarative
} // namespace Phonon
