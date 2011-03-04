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

#ifndef VIDEOEFFECTS_H
#define VIDEOEFFECTS_H

#include <QDialog>

namespace Ui {
    class VideoEffects;
}
namespace Phonon { class VideoWidget; }

class VideoEffects : public QDialog
{
    Q_OBJECT

public:
    explicit VideoEffects(Phonon::VideoWidget *, QWidget *parent = 0);
    ~VideoEffects();

private:
    Ui::VideoEffects *ui;
    Phonon::VideoWidget *videoWidget;

private slots:
    void on_saturationSlider_valueChanged(int value);
    void on_hueSlider_valueChanged(int value);
    void on_contrastSlider_valueChanged(int value);
    void on_brightnessSlider_valueChanged(int value);
};

#endif // VIDEOEFFECTS_H
