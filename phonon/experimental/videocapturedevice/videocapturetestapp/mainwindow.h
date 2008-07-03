/*
mainwindow.h - MainWindow class header

Copyright (c) 2005-2008 by Cláudio da Silveira Pinheiro   <taupter@gmail.com>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of
the License or (at your option) version 3 or any later version
accepted by the membership of KDE e.V. (or its successor approved
by the membership of KDE e.V.), which shall act as a proxy 
defined in Section 14 of version 3 of the license.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QImage>
#include <QTimer>

class QPixmap;
class KUrl;

#include "videodevicepool.h"
#include "ui_mainwidget.h"

class MainWindow : public QWidget
{

Q_OBJECT

public:

    MainWindow( QWidget *parent = 0 );

    ~MainWindow();


private slots:

    void deviceChanged( int );
    void inputChanged( int );
    void standardChanged( int );

    void brightnessChanged( int );
    void contrastChanged( int );
    void saturationChanged( int );
    void whitenessChanged( int );
    void hueChanged( int );

    void adjustBrightnessContrastChanged( bool );
    void colorCorrectionChanged( bool );
    void mirrorPreviewChanged( bool );

    void updateImage();

    void deviceRegistered( const QString & );
    void deviceUnregistered( const QString & );

    void takePhoto( bool );

private:

    Ui_MainWidget  *main_widget;

    Phonon::VideoCapture::VideoDevicePool *vdpool;

    QImage captured_frame;

    QTimer update_timer;

    void saveImage( const QPixmap&, const KUrl& );

    void setVideoInputParameters();

};

#endif
