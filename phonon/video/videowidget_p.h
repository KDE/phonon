/*
    Copyright (C) 2006-2007 Matthias Kretz <kretz@kde.org>

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

#ifndef PHONON_VIDEOWIDGET_P_H
#define PHONON_VIDEOWIDGET_P_H

#include "videowidget.h"
#include "abstractoutput_p.h"

#include <QHBoxLayout>

namespace Phonon {

class VideoWidgetInterface;

class VideoWidgetPrivate : public AbstractOutputPrivate
{
protected:
    /** \reimp */
    virtual void createBackendObject() Q_DECL_OVERRIDE Q_DECL_FINAL;

    /** \reimp */
    virtual void setupBackendObject() Q_DECL_OVERRIDE Q_DECL_FINAL;

    VideoWidgetPrivate(VideoWidget *parent)
        : layout(parent)
        , aspectRatio(VideoWidget::AspectRatioAuto)
        , scaleMode(VideoWidget::FitInView)
        , brightness(0)
        , contrast(0)
        , hue(0)
        , saturation(0)
        , interface(0)
    {
        layout.setMargin(0);
    }

    QHBoxLayout layout;
    VideoWidget::AspectRatio aspectRatio;
    VideoWidget::ScaleMode scaleMode;
    Qt::WindowFlags changeFlags;

    qreal brightness;
    qreal contrast;
    qreal hue;
    qreal saturation;

    VideoWidgetInterface *interface;

private:
    void init();

    P_DECLARE_PUBLIC(VideoWidget)
};

} // namespace Phonon

#endif // PHONON_VIDEOWIDGET_P_H
