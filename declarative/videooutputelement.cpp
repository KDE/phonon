/*
    Copyright (C) 2011-2012 Harald Sitter <sitter@kde.org>

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

#include "videooutputelement.h"

#include <QApplication>

#include <phonon/mediaobject.h>
#include <phonon/graphicsview/videographicsobject.h>

namespace Phonon {
namespace Declarative {

VideoOutputElement::VideoOutputElement(QDeclarativeItem *parent) :
    QDeclarativeItem(parent),
    m_graphicsObject(0),
    m_isFullScreen(false),
    m_spy(0)
{
}

VideoOutputElement::~VideoOutputElement()
{
}

void VideoOutputElement::classBegin()
{
    m_graphicsObject = new VideoGraphicsObject(this);
}

void VideoOutputElement::init(MediaObject *mediaObject)
{
#ifdef __GNUC__
#warning inheritance bad -> inits all the phonon without being used
#endif
    Q_ASSERT(mediaObject);

    m_mediaObject = mediaObject;
    createPath(m_mediaObject, m_graphicsObject);

    // Don't auto detect the spy if the user set one manually.
    if (!m_spy) {
        QDeclarativeItem *rootItem = parentItem();
        while (rootItem->parentItem())
            rootItem = rootItem->parentItem();

        bool foundSpy = false;
        foreach (QObject *qobject, rootItem->children()) {
            VideoFormatSpyElement *spy = qobject_cast<VideoFormatSpyElement *>(qobject);
            if (spy) {
                setSpy(spy);
                foundSpy = true;
                break;
            }
        }

        if (!foundSpy)
            setSpy(new VideoFormatSpyElement(rootItem));
    }

    initChildren(this);
}

bool VideoOutputElement::isCursorVisible() const
{
    return cursor().shape() == Qt::BlankCursor ? true : false;
}

void VideoOutputElement::setCursorVisible(bool visible)
{
    setCursor(visible ? QCursor() : QCursor(Qt::BlankCursor));
}

bool VideoOutputElement::isFullScreen() const
{
    return m_isFullScreen;
}

void VideoOutputElement::setFullScreen(bool fullScreen)
{
    if (fullScreen) {
        m_isFullScreen = true;
        qApp->activeWindow()->setWindowState(qApp->activeWindow()->windowState() | Qt::WindowFullScreen);
    } else {
        m_isFullScreen = false;
        qApp->activeWindow()->setWindowState(qApp->activeWindow()->windowState() & ~Qt::WindowFullScreen);
    }
    emit fullScreenChanged();
}

void VideoOutputElement::setSpy(VideoFormatSpyElement *spy)
{
    if (m_spy && m_spy != spy)
        disconnect(m_spy, 0, m_graphicsObject, 0);
    m_spy = spy;
    connect(m_spy, SIGNAL(formatsChanged(QMap<GraphicsPainterType,QList<VideoFrame::Format> >)),
            m_graphicsObject, SLOT(setSpyFormats(QMap<GraphicsPainterType,QList<VideoFrame::Format> >)));
    m_graphicsObject->setSpyFormats(m_spy->formats());
    emit spyChanged();
}

void VideoOutputElement::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    m_graphicsObject->setGeometry(newGeometry);
    QDeclarativeItem::geometryChanged(newGeometry, oldGeometry);
}

} // namespace Declarative
} // namespace Phonon
