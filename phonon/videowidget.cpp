/*
    Copyright (C) 2005-2007 Matthias Kretz <kretz@kde.org>
    Copyright (C) 2013 Harald Sitter <sitter@kde.org>

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

#include "videowidget.h"
#include "videowidget_p.h"
#include "videowidgetinterface.h"
#include "factory_p.h"
#include "phonondefs_p.h"
#include "phononnamespace_p.h"

#include <QAction>

#ifndef QT_NO_PHONON_VIDEO

namespace Phonon {

VideoWidget::VideoWidget(QWidget *parent)
    : QWidget(parent)
    , Phonon::AbstractVideoOutput(*new VideoWidgetPrivate(this))
{
    P_D(VideoWidget);
    d->init();
    d->createBackendObject();
    setMouseTracking(true);
}

VideoWidget::VideoWidget(VideoWidgetPrivate &dd, QWidget *parent)
    : QWidget(parent),
    Phonon::AbstractVideoOutput(dd)
{
    P_D(VideoWidget);
    d->init();
}

void VideoWidgetPrivate::init()
{
    P_Q(VideoWidget);
    changeFlags = q->windowFlags() & (Qt::SubWindow | Qt::Window);
}

void VideoWidget::mouseMoveEvent(QMouseEvent *e)
{
    QWidget::mouseMoveEvent(e);
}

void VideoWidgetPrivate::createBackendObject()
{
    if (m_backendObject)
        return;
    P_Q(VideoWidget);
    m_backendObject = Factory::createVideoWidget(q);
    interface = qobject_cast<VideoWidgetInterface *>(m_backendObject);
    if (m_backendObject && interface) // Need at least base interface.
        setupBackendObject();
}

VideoWidget::AspectRatio VideoWidget::aspectRatio() const
{
    P_D(const VideoWidget);
    if (d->interface)
        return d->interface->aspectRatio();
    return d->aspectRatio;
}

void VideoWidget::setAspectRatio(VideoWidget::AspectRatio aspectRatio)
{
    P_D(const VideoWidget);
    if (d->interface)
        d->interface->setAspectRatio(aspectRatio);
}

VideoWidget::ScaleMode VideoWidget::scaleMode() const
{
    P_D(const VideoWidget);
    if (d->interface)
        return d->interface->scaleMode();
    return d->scaleMode;
}

void VideoWidget::setScaleMode(VideoWidget::ScaleMode scaleMode)
{
    P_D(const VideoWidget);
    if (d->interface)
        d->interface->setScaleMode(scaleMode);
}

qreal VideoWidget::brightness() const
{
    P_D(const VideoWidget);
    if (d->interface)
        return d->interface->brightness();
    return d->brightness;
}

void VideoWidget::setBrightness(qreal brightness)
{
    P_D(const VideoWidget);
    if (d->interface)
        d->interface->setBrightness(brightness);
}

qreal VideoWidget::contrast() const
{
    P_D(const VideoWidget);
    if (d->interface)
        return d->interface->contrast();
    return d->contrast;
}

void VideoWidget::setContrast(qreal contrast)
{
    P_D(const VideoWidget);
    if (d->interface)
        d->interface->setContrast(contrast);
}

qreal VideoWidget::hue() const
{
    P_D(const VideoWidget);
    if (d->interface)
        return d->interface->hue();
    return d->hue;
}

void VideoWidget::setHue(qreal hue)
{
    P_D(const VideoWidget);
    if (d->interface)
        d->interface->setHue(hue);
}

qreal VideoWidget::saturation() const
{
    P_D(const VideoWidget);
    if (d->interface)
        return d->interface->saturation();
    return d->saturation;
}

void VideoWidget::setSaturation(qreal saturation)
{
    P_D(const VideoWidget);
    if (d->interface)
        d->interface->setSaturation(saturation);
}

QImage VideoWidget::snapshot() const {
    P_D(const VideoWidget);
    if (d->interface)
        return d->interface->snapshot();
    return QImage(); // TODO not implemented in VideoInterface
}

void VideoWidget::setFullScreen(bool newFullScreen)
{
    pDebug() << Q_FUNC_INFO << newFullScreen;
    P_D(VideoWidget);
    // TODO: disable screensaver? or should we leave that responsibility to the
    // application?
    Qt::WindowFlags flags = windowFlags();
    if (newFullScreen) {
        if (!isFullScreen()) {
            //we only update that value if it is not already fullscreen
            d->changeFlags = flags & (Qt::Window | Qt::SubWindow);
            flags |= Qt::Window;
            flags ^= Qt::SubWindow;
            setWindowFlags(flags);
#ifdef Q_WS_X11
            // This works around a bug with Compiz
            // as the window must be visible before we can set the state
            show();
            raise();
            setWindowState( windowState() | Qt::WindowFullScreen ); // set
#else
            setWindowState( windowState() | Qt::WindowFullScreen ); // set
            show();
#endif
        }
    } else if (isFullScreen()) {
        flags ^= (Qt::Window | Qt::SubWindow); //clear the flags...
        flags |= d->changeFlags; //then we reset the flags (window and subwindow)
        setWindowFlags(flags);
        setWindowState( windowState()  ^ Qt::WindowFullScreen ); // reset
        show();
    }
}

void VideoWidget::exitFullScreen()
{
    setFullScreen(false);
}

void VideoWidget::enterFullScreen()
{
    setFullScreen(true);
}

bool VideoWidgetPrivate::aboutToDeleteBackendObject()
{
    aspectRatio = interface->aspectRatio();
    scaleMode = interface->scaleMode();
    return AbstractVideoOutputPrivate::aboutToDeleteBackendObject();
}

void VideoWidgetPrivate::setupBackendObject()
{
    P_Q(VideoWidget);
    Q_ASSERT(m_backendObject);
    //AbstractVideoOutputPrivate::setupBackendObject();
    pDebug() << "calling setAspectRatio on the backend " << aspectRatio;
    interface->setAspectRatio(aspectRatio);
    interface->setScaleMode(scaleMode);

    QWidget *w = interface->widget();
    if (w) {
        layout.addWidget(w);
        q->setSizePolicy(w->sizePolicy());
        w->setMouseTracking(true);
    }
}

bool VideoWidget::event(QEvent *e)
{
    return QWidget::event(e);
}

} //namespace Phonon

#endif //QT_NO_PHONON_VIDEO

#include "moc_videowidget.cpp"
