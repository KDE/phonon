/*
    Copyright (C) 2005-2007 Matthias Kretz <kretz@kde.org>

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

#ifndef PHONON_VIDEOWIDGET_H
#define PHONON_VIDEOWIDGET_H

#include "phonon_export.h"
#include "abstractoutput.h"

#include <QWidget>

class QString;

namespace Phonon {

class AbstractVideoOutput;
class VideoWidgetPrivate;

class PHONON_EXPORT VideoWidget : public QWidget, public AbstractOutput
{
    Q_OBJECT
    Q_ENUMS(AspectRatio ScaleMode)
    Q_PROPERTY(bool fullScreen READ isFullScreen WRITE setFullScreen)
    Q_PROPERTY(AspectRatio aspectRatio READ aspectRatio WRITE setAspectRatio)
    Q_PROPERTY(ScaleMode scaleMode READ scaleMode WRITE setScaleMode)
    Q_PROPERTY(qreal brightness READ brightness WRITE setBrightness)
    Q_PROPERTY(qreal contrast READ contrast WRITE setContrast)
    Q_PROPERTY(qreal hue READ hue WRITE setHue)
    Q_PROPERTY(qreal saturation READ saturation WRITE setSaturation)
public:
    enum AspectRatio {
        AspectRatioAuto = 0,
        AspectRatioWidget = 1,
        AspectRatio4_3 = 2,
        AspectRatio16_9 = 3
        //X                 /**
        //X                  * Assume that every pixel of the video image needs to be displayed with the same
        //X                  * physical width and height. (1:1 image pixels, not imagewidth
        //X                  * = imageheight)
        //X                  */
        //X                 AspectRatioSquare = 4,
    };

    enum ScaleMode {
        FitInView = 0,
        ScaleAndCrop = 1
    };

    VideoWidget(QWidget *parent = 0);

    AspectRatio aspectRatio() const;
    ScaleMode scaleMode() const;

    qreal brightness() const;
    qreal contrast() const;
    qreal hue() const;
    qreal saturation() const;
    QImage snapshot() const;

public Q_SLOTS:
    void setFullScreen(bool fullscreen);
    void exitFullScreen();
    void enterFullScreen();

    void setAspectRatio(AspectRatio);
    void setScaleMode(ScaleMode);

    void setBrightness(qreal value);
    void setContrast(qreal value);
    void setHue(qreal value);
    void setSaturation(qreal value);

protected:
    VideoWidget(VideoWidgetPrivate &d, QWidget *parent);

    void mouseMoveEvent(QMouseEvent *);
    bool event(QEvent *);

private:
    P_DECLARE_PRIVATE(VideoWidget)
};

} // namespace Phonon

#endif // PHONON_VIDEOWIDGET_H
