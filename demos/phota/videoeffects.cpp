/*
    Copyright (C) 2010 Jean-Baptiste Kempf <jb@videolan.org>

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

#include "videoeffects.h"
#include "ui_videoeffects.h"
#include <assert.h>

//#include <Phonon/VideoWidget>
#include <phonon/videowidget.h>

VideoEffects::VideoEffects(Phonon::VideoWidget *_w, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::VideoEffects),
    videoWidget(_w)
{
    ui->setupUi(this);
    assert(_w);
}

VideoEffects::~VideoEffects()
{
    delete ui;
}

void VideoEffects::on_brightnessSlider_valueChanged(int value)
{
    videoWidget->setBrightness(qreal(value)/100);
}

void VideoEffects::on_contrastSlider_valueChanged(int value)
{
    videoWidget->setContrast(qreal(value)/100);
}

void VideoEffects::on_hueSlider_valueChanged(int value)
{
    videoWidget->setHue(qreal(value)/100);
}

void VideoEffects::on_saturationSlider_valueChanged(int value)
{
    videoWidget->setSaturation(qreal(value)/100);
}
