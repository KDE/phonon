/*  This file is part of the KDE project
    Copyright (C) 2006-2007 Matthias Kretz <kretz@kde.org>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.

*/

#include "videowidget.h"
#include "events.h"
#include <QPalette>
#include <QtCore/QExplicitlySharedDataPointer>
#include <QtCore/QMutexLocker>
#include <QtCore/QSharedData>
#include <QImage>
#include <QPainter>
#include <kdebug.h>

//#ifndef PHONON_XINE_NO_VIDEOWIDGET
#include <QX11Info>
#include <xcb/xcb.h>
//#endif // PHONON_XINE_NO_VIDEOWIDGET

#include "backend.h"
#include "xineengine.h"
#include "mediaobject.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QMouseEvent>
#include <QVBoxLayout>
#include "keepreference.h"

namespace Phonon
{
namespace Xine
{

static XcbConnection *s_instance = 0;

QExplicitlySharedDataPointer<XcbConnection> XcbConnection::instance()
{
    kDebug();
    if (!s_instance) {
        new XcbConnection;
    }
    Q_ASSERT(s_instance);
    return QExplicitlySharedDataPointer<XcbConnection>(s_instance);
}

XcbConnection::~XcbConnection()
{
    kDebug();
    s_instance = 0;

//#ifndef PHONON_XINE_NO_VIDEOWIDGET
    xcb_disconnect(m_xcbConnection);
    m_xcbConnection = 0;
//#endif // PHONON_XINE_NO_VIDEOWIDGET
}

XcbConnection::XcbConnection()
    : m_screen(0)
{
    kDebug();
    Q_ASSERT(!s_instance);
    s_instance = this;
    int preferredScreen = 0;
//#ifndef PHONON_XINE_NO_VIDEOWIDGET
    m_xcbConnection = xcb_connect(NULL, &preferredScreen);//DisplayString(x11Info().display()), NULL);
    if (m_xcbConnection) {
        xcb_screen_iterator_t screenIt = xcb_setup_roots_iterator(xcb_get_setup(m_xcbConnection));
        while ((screenIt.rem > 1) && (preferredScreen > 0)) {
            xcb_screen_next(&screenIt);
            --preferredScreen;
        }
        m_screen = screenIt.data;
    }
//#endif // PHONON_XINE_NO_VIDEOWIDGET
}

//#ifndef PHONON_XINE_NO_VIDEOWIDGET
static void dest_size_cb(void *user_data, int video_width, int video_height, double video_pixel_aspect,
        int *dest_width, int *dest_height, double *dest_pixel_aspect)
{
    Phonon::Xine::VideoWidgetXT *xt = static_cast<VideoWidgetXT *>(user_data);
    if (!xt->videoWidget()) {
        *dest_width = 1;
        *dest_height = 1;
        *dest_pixel_aspect = 1.0;
        return;
    }

    int win_x, win_y;
    xt->videoWidget()->xineCallback(win_x, win_y, *dest_width, *dest_height, *dest_pixel_aspect,
            video_width, video_height, video_pixel_aspect, false);
}

static void frame_output_cb(void *user_data, int video_width, int video_height,
        double video_pixel_aspect, int *dest_x, int *dest_y,
        int *dest_width, int *dest_height,
        double *dest_pixel_aspect, int *win_x, int *win_y)
{
    Phonon::Xine::VideoWidgetXT *xt = static_cast<VideoWidgetXT *>(user_data);
    if (!xt->videoWidget()) {
        *win_x = 0;
        *win_y = 0;
        *dest_width = 1;
        *dest_height = 1;
        *dest_pixel_aspect = 1.0;
        return;
    }

    xt->videoWidget()->xineCallback(*win_x, *win_y, *dest_width, *dest_height, *dest_pixel_aspect,
            video_width, video_height, video_pixel_aspect, true);

    *dest_x = 0;
    *dest_y = 0;
}
//#endif // PHONON_XINE_NO_VIDEOWIDGET

void VideoWidget::xineCallback(int &x, int &y, int &width, int &height, double &ratio,
        int videoWidth, int videoHeight, double videoRatio, bool mayResize)
{
    Q_UNUSED(videoRatio);
    Q_UNUSED(videoWidth);
    Q_UNUSED(videoHeight);
    Q_UNUSED(mayResize);

    x = this->x();
    y = this->y();
    width = this->width();
    height = this->height();

    // square pixels
    ratio = 1.0;
}

VideoWidgetXT::VideoWidgetXT(VideoWidget *w)
    : SinkNodeXT("VideoWidget"),
    m_xcbConnection(0), //XcbConnection::instance()),
    m_videoPort(0), m_videoWidget(w), m_isValid(false)
{
    Q_ASSERT(!m_xine);
    m_xine = Backend::xine();
}

void VideoWidgetXT::createVideoPort()
{
//#ifndef PHONON_XINE_NO_VIDEOWIDGET
    Q_ASSERT(!m_videoPort);
    int preferredScreen = 0;
    m_xcbConnection = xcb_connect(NULL, &preferredScreen);//DisplayString(x11Info().display()), NULL);
    kDebug() << "xcb_connect" << m_xcbConnection;
    if (m_xcbConnection && m_xine) {
        xcb_screen_iterator_t screenIt = xcb_setup_roots_iterator(xcb_get_setup(m_xcbConnection));
        while ((screenIt.rem > 1) && (preferredScreen > 0)) {
            xcb_screen_next(&screenIt);
            --preferredScreen;
        }
        m_visual.connection = m_xcbConnection;
        //Q_ASSERT(m_xcbConnection->screen());
        //m_visual.screen = m_xcbConnection->screen();
        m_visual.screen = screenIt.data;
        m_visual.window = m_videoWidget->winId();
        m_visual.user_data = static_cast<void *>(this);
        m_visual.dest_size_cb = Phonon::Xine::dest_size_cb;
        m_visual.frame_output_cb = Phonon::Xine::frame_output_cb;

        // make sure all Qt<->X communication is done, else xine_open_video_driver will crash
        QApplication::syncX();

        Q_ASSERT(m_videoWidget->testAttribute(Qt::WA_WState_Created));
        m_videoPort = xine_open_video_driver(m_xine, "auto", XINE_VISUAL_TYPE_XCB, static_cast<void *>(&m_visual));
        if (!m_videoPort) {
//#endif // PHONON_XINE_NO_VIDEOWIDGET
            m_videoPort = xine_open_video_driver(m_xine, "auto", XINE_VISUAL_TYPE_NONE, 0);
            kError(610) << "No xine video output plugin using libxcb for threadsafe access to the X server found. No video for you.";
//#ifndef PHONON_XINE_NO_VIDEOWIDGET
        }
    }
//#endif // PHONON_XINE_NO_VIDEOWIDGET
}

VideoWidget::VideoWidget(QWidget *parent)
    : QWidget(parent),
    SinkNode(new VideoWidgetXT(this)),
    m_aspectRatio(Phonon::VideoWidget::AspectRatioAuto),
    m_scaleMode(Phonon::VideoWidget::FitInView),
    m_empty(true),
    m_brightness(0.0),
    m_contrast(0.0),
    m_hue(0.0),
    m_saturation(0.0)
{
    // for some reason it can hang if the widget is 0x0
    setMinimumSize(1, 1);

    QPalette palette = this->palette();
    palette.setColor(backgroundRole(), Qt::black);
    setPalette(palette);

    // when resizing fill with black (backgroundRole color) the rest is done by paintEvent
    setAttribute(Qt::WA_OpaquePaintEvent, true);

    // disable Qt composition management as Xine draws onto the widget directly using X calls
    setAttribute(Qt::WA_PaintOnScreen, true);

    K_XT(VideoWidget);
    xt->createVideoPort();

    // required for dvdnav
    setMouseTracking(true);
}

VideoWidget::~VideoWidget()
{
    kDebug(610);
    K_XT(VideoWidget);
    xt->m_videoWidget = 0;
    if (xt->m_videoPort) {
        xine_port_send_gui_data(xt->m_videoPort, XINE_GUI_SEND_WILL_DESTROY_DRAWABLE, 0);
    }
}

VideoWidgetXT::~VideoWidgetXT()
{
    kDebug(610);
    if (m_videoPort && m_xine) {
        xine_close_video_driver(m_xine, m_videoPort);
    }
    if (m_xcbConnection) {
        kDebug() << "xcb_disconnect" << m_xcbConnection;
        xcb_disconnect(m_xcbConnection);
        m_xcbConnection = 0;
    }
}

Phonon::VideoWidget::AspectRatio VideoWidget::aspectRatio() const
{
    return m_aspectRatio;
}

void VideoWidget::setAspectRatio(Phonon::VideoWidget::AspectRatio aspectRatio)
{
    m_aspectRatio = aspectRatio;
    switch (m_aspectRatio) {
    case Phonon::VideoWidget::AspectRatioWidget:
        upstreamEvent(new SetParamEvent(XINE_PARAM_VO_ASPECT_RATIO, XINE_VO_ASPECT_SQUARE));
        break;
    case Phonon::VideoWidget::AspectRatioAuto:
        upstreamEvent(new SetParamEvent(XINE_PARAM_VO_ASPECT_RATIO, XINE_VO_ASPECT_AUTO));
        break;
    case Phonon::VideoWidget::AspectRatio4_3:
        upstreamEvent(new SetParamEvent(XINE_PARAM_VO_ASPECT_RATIO, XINE_VO_ASPECT_4_3));
        break;
    case Phonon::VideoWidget::AspectRatio16_9:
        upstreamEvent(new SetParamEvent(XINE_PARAM_VO_ASPECT_RATIO, XINE_VO_ASPECT_ANAMORPHIC));
        break;
    }
    updateZoom();
}

Phonon::VideoWidget::ScaleMode VideoWidget::scaleMode() const
{
    return m_scaleMode;
}

void VideoWidget::setScaleMode(Phonon::VideoWidget::ScaleMode mode)
{
    m_scaleMode = mode;
    updateZoom();
}

qreal VideoWidget::brightness() const
{
    return m_brightness;
}

static const qreal ONE = 1.0;
void VideoWidget::setBrightness(qreal newBrightness)
{
    newBrightness = qBound(-ONE, newBrightness, ONE);
    if (m_brightness != newBrightness) {
        m_brightness = newBrightness;
        upstreamEvent(new SetParamEvent(XINE_PARAM_VO_BRIGHTNESS, static_cast<int>(0x7fff * (m_brightness + ONE))));
    }
}

qreal VideoWidget::contrast() const
{
    return m_contrast;
}

void VideoWidget::setContrast(qreal newContrast)
{
    newContrast = qBound(-ONE, newContrast, ONE);
    if (m_contrast != newContrast) {
        m_contrast = newContrast;
        upstreamEvent(new SetParamEvent(XINE_PARAM_VO_CONTRAST, static_cast<int>(0x7fff * (m_contrast + ONE))));
    }
}

qreal VideoWidget::hue() const
{
    return m_hue;
}

void VideoWidget::setHue(qreal newHue)
{
    newHue = qBound(-ONE, newHue, ONE);
    if (m_hue != newHue) {
        m_hue = newHue;
        upstreamEvent(new SetParamEvent(XINE_PARAM_VO_HUE, static_cast<int>(0x7fff * (m_hue + ONE))));
    }
}

qreal VideoWidget::saturation() const
{
    return m_saturation;
}

void VideoWidget::setSaturation(qreal newSaturation)
{
    newSaturation = qBound(-ONE, newSaturation, ONE);
    if (m_saturation != newSaturation) {
        m_saturation = newSaturation;
        upstreamEvent(new SetParamEvent(XINE_PARAM_VO_SATURATION, static_cast<int>(0x7fff * (m_saturation + ONE))));
    }
}

QImage VideoWidget::snapshot() const
{
    QImage img;
    QMutexLocker lock(&m_snapshotLock);
    const_cast<VideoWidget *>(this)->upstreamEvent(new Event(Event::RequestSnapshot));
    if (m_snapshotWait.wait(&m_snapshotLock, 1000)) {
        img = m_snapshotImage;
        m_snapshotImage = QImage();
    }
    return img;
}

/*
int VideoWidget::overlayCapabilities() const
{
    return Phonon::Experimental::OverlayApi::OverlayOpaque;
}

bool VideoWidget::createOverlay(QWidget *widget, int type)
{
    if ((overlay != 0) || (type != Phonon::Experimental::OverlayApi::OverlayOpaque))
        return false;

    if (layout() == 0) {
        QLayout *layout = new QVBoxLayout(this);
        layout->setMargin(0);
        setLayout(layout);
    }

    layout()->addWidget(widget);
    overlay = widget;

    return true;
}

void VideoWidget::childEvent(QChildEvent *event)
{
    if (event->removed() && (event->child() == overlay))
        overlay = 0;
    QWidget::childEvent(event);
}
*/

void VideoWidget::updateZoom()
{
    if (m_aspectRatio == Phonon::VideoWidget::AspectRatioWidget) {
        const QSize s = size();
        QSize imageSize = m_sizeHint;
        imageSize.scale(s, Qt::KeepAspectRatio);
        if (imageSize.width() < s.width()) {
            const int zoom = s.width() * 100 / imageSize.width();
            upstreamEvent(new SetParamEvent(XINE_PARAM_VO_ZOOM_X, zoom));
            upstreamEvent(new SetParamEvent(XINE_PARAM_VO_ZOOM_Y, 100));
        } else {
            const int zoom = s.height() * 100 / imageSize.height();
            upstreamEvent(new SetParamEvent(XINE_PARAM_VO_ZOOM_X, 100));
            upstreamEvent(new SetParamEvent(XINE_PARAM_VO_ZOOM_Y, zoom));
        }
    } else if (m_scaleMode == Phonon::VideoWidget::ScaleAndCrop) {
        const QSize s = size();
        QSize imageSize = m_sizeHint;
        // the image size is in square pixels
        // first transform it to the current aspect ratio
        kDebug(610) << imageSize;
        switch (m_aspectRatio) {
        case Phonon::VideoWidget::AspectRatioAuto:
            // FIXME: how can we find out the ratio xine decided on? the event?
            break;
        case Phonon::VideoWidget::AspectRatio4_3:
            imageSize.setWidth(imageSize.height() * 4 / 3);
            break;
        case Phonon::VideoWidget::AspectRatio16_9:
            imageSize.setWidth(imageSize.height() * 16 / 9);
            break;
        default:
            // correct ratio already
            break;
        }
        kDebug(610) << imageSize;
        imageSize.scale(s, Qt::KeepAspectRatioByExpanding);
        kDebug(610) << imageSize << s;
        int zoom;
        if (imageSize.width() > s.width()) {
            zoom = imageSize.width() * 100 / s.width();
        } else {
            zoom = imageSize.height() * 100 / s.height();
        }
        upstreamEvent(new SetParamEvent(XINE_PARAM_VO_ZOOM_X, zoom));
        upstreamEvent(new SetParamEvent(XINE_PARAM_VO_ZOOM_Y, zoom));
    } else {
        upstreamEvent(new SetParamEvent(XINE_PARAM_VO_ZOOM_X, 100));
        upstreamEvent(new SetParamEvent(XINE_PARAM_VO_ZOOM_Y, 100));
    }
}

void VideoWidget::resizeEvent(QResizeEvent *ev)
{
    updateZoom();
    QWidget::resizeEvent(ev);
}

bool VideoWidget::event(QEvent *ev)
{
    switch (ev->type()) {
    case Event::NavButtonIn:
        kDebug(610) << "NavButtonIn";
        setCursor(QCursor(Qt::PointingHandCursor));
        ev->accept();
        return true;
    case Event::NavButtonOut:
        kDebug(610) << "NavButtonOut";
        unsetCursor();
        ev->accept();
        return true;
    case Event::FrameFormatChange:
        ev->accept();
        {
            FrameFormatChangeEvent *e = static_cast<FrameFormatChangeEvent *>(ev);
            kDebug(610) << "FrameFormatChangeEvent " << e->size;
            m_sizeHint = e->size;
            updateGeometry();
        }
        return true;
    default:
        return QWidget::event(ev);
    }
}

void VideoWidget::mouseMoveEvent(QMouseEvent *mev)
{
    K_XT(VideoWidget);

    if (cursor().shape() == Qt::BlankCursor) {
        setCursor(QCursor(Qt::ArrowCursor));
    }

    x11_rectangle_t   rect;
    xine_event_t      *event = new xine_event_t;
    xine_input_data_t *input = new xine_input_data_t;

    rect.x = mev->x();
    rect.y = mev->y();
    rect.w = 0;
    rect.h = 0;

    xine_port_send_gui_data(xt->m_videoPort, XINE_GUI_SEND_TRANSLATE_GUI_TO_VIDEO, (void *) &rect);

    event->type        = XINE_EVENT_INPUT_MOUSE_MOVE;
    event->data        = input;
    event->data_length = sizeof(*input);
    input->button      = 0;
    input->x           = rect.x;
    input->y           = rect.y;
    //kDebug(610) << "upstreamEvent(EventSendEvent(move " << rect.x << rect.y;
    upstreamEvent(new EventSendEvent(event));

    QWidget::mouseMoveEvent(mev);
}

void VideoWidget::mousePressEvent(QMouseEvent *mev)
{
    K_XT(VideoWidget);

    uint8_t button = 1;
    switch (mev->button()) {
    case Qt::NoButton:
    case Qt::XButton1:
    case Qt::XButton2:
    case Qt::MouseButtonMask:
        break;
    case Qt::RightButton: // 3
        ++button;
    case Qt::MidButton: // 2
        ++button;
    case Qt::LeftButton: // 1
        {
            x11_rectangle_t   rect;
            xine_event_t      *event = new xine_event_t;
            xine_input_data_t *input = new xine_input_data_t;

            rect.x = mev->x();
            rect.y = mev->y();
            rect.w = 0;
            rect.h = 0;

            xine_port_send_gui_data(xt->m_videoPort, XINE_GUI_SEND_TRANSLATE_GUI_TO_VIDEO, (void *) &rect);

            event->type        = XINE_EVENT_INPUT_MOUSE_BUTTON;
            event->data        = input;
            event->data_length = sizeof(*input);
            input->button      = button;
            input->x           = rect.x;
            input->y           = rect.y;
            //kDebug(610) << "upstreamEvent(EventSendEvent(button " << rect.x << rect.y;
            upstreamEvent(new EventSendEvent(event));
        }
    }
    QWidget::mousePressEvent(mev);
}

bool VideoWidget::isValid() const
{
    return true;
    //K_XT(const VideoWidget);
    //return xt->m_isValid;
}

xine_video_port_t *VideoWidgetXT::videoPort() const
{
    return m_videoPort;
}

void VideoWidgetXT::rewireTo(SourceNodeXT *source)
{
    if (!source->videoOutputPort()) {
        return;
    }
    xine_post_wire_video_port(source->videoOutputPort(), videoPort());
}

void VideoWidget::paintEvent(QPaintEvent *event)
{
    K_XT(VideoWidget);

    //kDebug(610) << "m_empty = " << m_empty;
    if (m_empty || !source()) {// || m_path->mediaObject()->state() == Phonon::LoadingState) {
        QPainter p(this);
        p.fillRect(rect(), Qt::black);
//#ifndef PHONON_XINE_NO_VIDEOWIDGET
    } else if (xt->m_videoPort) {
        //kDebug();
        const QRect &rect = event->rect();

        xcb_expose_event_t xcb_event;
        memset(&xcb_event, 0, sizeof(xcb_event));

        xcb_event.window = winId();
        xcb_event.x = rect.x();
        xcb_event.y = rect.y();
        xcb_event.width = rect.width();
        xcb_event.height = rect.height();
        xcb_event.count = 0;

        xine_port_send_gui_data(xt->m_videoPort, XINE_GUI_SEND_EXPOSE_EVENT, &xcb_event);
//#endif // PHONON_XINE_NO_VIDEOWIDGET
    } else {
        QPainter p(this);
        p.fillRect(rect(), Qt::black);
    }
    QWidget::paintEvent(event);
}

void VideoWidget::showEvent(QShowEvent *)
{
    //K_XT(VideoWidget);
    //xine_port_send_gui_data(xt->m_videoPort, XINE_GUI_SEND_VIDEOWIN_VISIBLE, static_cast<void *>(1));
}

void VideoWidget::hideEvent(QHideEvent *)
{
    //K_XT(VideoWidget);
    //xine_port_send_gui_data(xt->m_videoPort, XINE_GUI_SEND_VIDEOWIN_VISIBLE, static_cast<void *>(0));
}

void VideoWidget::changeEvent(QEvent *event)
{
    K_XT(VideoWidget);

    if (event->type() == QEvent::ParentAboutToChange)
    {
        kDebug(610) << "ParentAboutToChange";
    }
    else if (event->type() == QEvent::ParentChange)
    {
        kDebug(610) << "ParentChange" << winId();
//#ifndef PHONON_XINE_NO_VIDEOWIDGET
        if (xt->m_visual.window != winId()) {
            xt->m_visual.window = winId();
            if (xt->m_videoPort) {
                // make sure all Qt<->X communication is done, else winId() might not be known at the
                // X-server yet
                QApplication::syncX();
                xine_port_send_gui_data(xt->m_videoPort, XINE_GUI_SEND_DRAWABLE_CHANGED, reinterpret_cast<void *>(xt->m_visual.window));
                kDebug(610) << "XINE_GUI_SEND_DRAWABLE_CHANGED done.";
            }
        }
//#endif // PHONON_XINE_NO_VIDEOWIDGET
    }
}

void VideoWidget::downstreamEvent(Event *e)
{
    Q_ASSERT(e);
    switch (e->type()) {
    case Event::HasVideo:
        {
            HasVideoEvent *ev = static_cast<HasVideoEvent *>(e);
            m_empty = !ev->hasVideo;
            if (m_empty) {
                update();
            }
        }
        break;
    case Event::SnapshotReady:
        m_snapshotLock.lock();
        m_snapshotImage = static_cast<const SnapshotReadyEvent *>(e)->image;
        m_snapshotWait.wakeAll();
        m_snapshotLock.unlock();
        break;
    default:
        QCoreApplication::sendEvent(this, e);
        break;
    }
    SinkNode::downstreamEvent(e);
}

void VideoWidget::aboutToChangeXineEngine()
{
    kDebug();
    K_XT(VideoWidget);
    if (xt->m_videoPort) {
        VideoWidgetXT *xt2 = new VideoWidgetXT(this);
        xt2->m_xine = xt->m_xine;
        xt2->m_videoPort = xt->m_videoPort;
        xt2->m_xcbConnection = xt->m_xcbConnection;
        xt->m_videoPort = 0;
        xt->m_xcbConnection = 0;
        KeepReference<> *keep = new KeepReference<>;
        keep->addObject(xt2);
        keep->ready();
    }
}

void VideoWidget::xineEngineChanged()
{
    kDebug();
    K_XT(VideoWidget);
    if (xt->m_xine) {
        Q_ASSERT(!xt->m_videoPort);
        xt->createVideoPort();
    }
}

}} //namespace Phonon::Xine

#include "videowidget.moc"
// vim: sw=4 ts=4
