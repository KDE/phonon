/*
mainwindow.cpp - MainWindow class implementation

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

#include "mainwindow.h"

#include <QImage>
#include <QPixmap>
#include <QDir>
#include <QImageWriter>

#include <KPushButton>
#include <KComboBox>
#include <KTemporaryFile>
#include <KHelpMenu>
#include <KMenu>
#include <KDialog>
#include <KApplication>
#include <KImageIO>
#include <KFileDialog>
#include <KImageFilePreview>
#include <KUrl>
#include <KMessageBox>
#include <KIO/NetAccess>

MainWindow::MainWindow( QWidget *parent )
: QWidget( parent )
{
    main_widget = new Ui_MainWidget();
    main_widget->setupUi( this );

    main_widget->video_image_label->setScaledContents( false );
    main_widget->video_image_label->setPixmap( KIcon( "videocapturetestapp" ).pixmap( 128, 128 ) );

    connect( main_widget->device_combo_box, SIGNAL( activated( int ) ), this, SLOT( deviceChanged( int ) ) );
    connect( main_widget->input_combo_box, SIGNAL( activated( int ) ), this, SLOT( inputChanged( int ) ) );

    connect( main_widget->standard_combo_box, SIGNAL( activated( int ) ), this, SLOT( standardChanged( int ) ) );
    connect( main_widget->brightness_slider, SIGNAL( valueChanged( int ) ), this, SLOT( brightnessChanged( int ) ) );
    connect( main_widget->contrast_slider, SIGNAL( valueChanged( int ) ), this,  SLOT( contrastChanged( int ) ) );
    connect( main_widget->saturation_slider, SIGNAL( valueChanged( int ) ), this, SLOT( saturationChanged( int ) ) );
    connect( main_widget->whiteness_slider, SIGNAL( valueChanged( int ) ), this, SLOT( whitenessChanged( int ) ) );
    connect( main_widget->hue_slider, SIGNAL( valueChanged( int ) ), this, SLOT( hueChanged( int ) ) );

    connect( main_widget->adjust_brightness_combo_box, SIGNAL( toggled( bool ) ), this, SLOT( adjustBrightnessContrastChanged( bool ) ) );
    connect( main_widget->correct_colors_combo_box, SIGNAL( toggled( bool ) ), this, SLOT( colorCorrectionChanged( bool ) ) );
    connect( main_widget->mirror_preview_check_box, SIGNAL( toggled( bool ) ), this, SLOT( mirrorPreviewChanged( bool ) ) );

    connect( main_widget->take_photo_button, SIGNAL( clicked( bool ) ), this, SLOT( takePhoto( bool ) ) );
    connect(main_widget->quit_button, SIGNAL( clicked() ), KApplication::kApplication(), SLOT( quit() ) );

    vdpool = Phonon::VideoCapture::VideoDevicePool::self();
    vdpool->open();
    vdpool->setSize( 320, 240 );

    vdpool->fillDeviceKComboBox( main_widget->device_combo_box );
    vdpool->fillInputKComboBox( main_widget->input_combo_box );
    vdpool->fillStandardKComboBox( main_widget->standard_combo_box );

    vdpool->setBrightness( 0 );
    vdpool->setHue( 0 );

    setVideoInputParameters();

    vdpool->startCapturing();

    connect( vdpool, SIGNAL( deviceRegistered( const QString & ) ), SLOT( deviceRegistered( const QString & ) ) );
    connect( vdpool, SIGNAL( deviceUnregistered( const QString & ) ), SLOT( deviceUnregistered( const QString & ) ) );

    connect( &update_timer, SIGNAL( timeout() ), this, SLOT( updateImage() ) );

    if( vdpool->hasDevices() ) {
        update_timer.start( 80 );
        main_widget->video_image_label->setScaledContents( true );
    }

    KHelpMenu *help_menu = new KHelpMenu( this, KGlobal::mainComponent().aboutData(), false );
    main_widget->help_button->setMenu( help_menu->menu() );
    main_widget->help_button->setIcon( KIcon( "help-contents" ) );
    main_widget->quit_button->setIcon( KIcon( "application-exit" ) );
    main_widget->take_photo_button->setIcon( KIcon( "videocapturetestapp" ) );

}


MainWindow::~MainWindow()
{
    vdpool->close();
}


void MainWindow::setVideoInputParameters()
{
    if( vdpool->size() ) {
        main_widget->brightness_slider->setValue( ( int )( vdpool->getBrightness() * main_widget->brightness_slider->maximum() ) );
        main_widget->contrast_slider->setValue( ( int )( vdpool->getContrast() * main_widget->contrast_slider->maximum() ) );
        main_widget->saturation_slider->setValue( ( int )( vdpool->getSaturation() * main_widget->saturation_slider->maximum() ) );
        main_widget->whiteness_slider->setValue( ( int )( vdpool->getWhiteness() * main_widget->whiteness_slider->maximum() ) );
        main_widget->hue_slider->setValue( ( int )( vdpool->getHue() * main_widget->hue_slider->maximum() ) );
        main_widget->adjust_brightness_combo_box->setChecked( vdpool->getAutoBrightnessContrast() );
        main_widget->correct_colors_combo_box->setChecked( vdpool->getAutoColorCorrection() );
        main_widget->mirror_preview_check_box->setChecked( vdpool->getImageAsMirror() );
    }
}

void MainWindow::deviceChanged( int )
{
    int new_device = main_widget->device_combo_box->currentIndex();

    if( ( new_device < vdpool->m_videodevice.size() ) && ( new_device != vdpool->currentDevice() ) ) {
        vdpool->open( new_device );
        vdpool->setSize( 320, 240 );
        vdpool->fillInputKComboBox( main_widget->input_combo_box );
        vdpool->startCapturing();
        setVideoInputParameters();
    }
}

void MainWindow::inputChanged( int )
{
    int new_input = main_widget->input_combo_box->currentIndex();

    if( ( new_input < vdpool->inputs() ) && ( new_input != vdpool->currentInput() ) ) {
        vdpool->selectInput( main_widget->input_combo_box->currentIndex() );
        vdpool->fillStandardKComboBox( main_widget->standard_combo_box );
        setVideoInputParameters();
    }
}

void MainWindow::standardChanged( int )
{
}

void MainWindow::brightnessChanged( int )
{
    vdpool->setBrightness( main_widget->brightness_slider->value() / (float)main_widget->brightness_slider->maximum() );
}

void MainWindow::contrastChanged( int )
{
    vdpool->setContrast( main_widget->contrast_slider->value() / (float)main_widget->contrast_slider->maximum() );
}

void MainWindow::saturationChanged( int )
{
    vdpool->setSaturation( main_widget->saturation_slider->value() / (float)main_widget->saturation_slider->maximum() );
}

void MainWindow::whitenessChanged( int )
{
    vdpool->setWhiteness( main_widget->whiteness_slider->value() / (float)main_widget->whiteness_slider->maximum() );
}

void MainWindow::hueChanged( int )
{
    vdpool->setHue( main_widget->hue_slider->value() / (float)main_widget->hue_slider->maximum() );
}

void MainWindow::adjustBrightnessContrastChanged( bool )
{
    vdpool->setAutoBrightnessContrast( main_widget->adjust_brightness_combo_box->isChecked() );
}

void MainWindow::colorCorrectionChanged( bool )
{
    vdpool->setAutoColorCorrection( main_widget->correct_colors_combo_box->isChecked() );
}

void MainWindow::mirrorPreviewChanged( bool )
{
    vdpool->setImageAsMirror( main_widget->mirror_preview_check_box->isChecked() );
}

void MainWindow::updateImage()
{
    vdpool->getFrame();
    vdpool->getImage( &captured_frame );
    main_widget->video_image_label->setPixmap( QPixmap::fromImage( captured_frame.mirrored( vdpool->getImageAsMirror(), false ) ) );
}

void MainWindow::deviceRegistered( const QString& )
{
    vdpool->fillDeviceKComboBox( main_widget->device_combo_box );
    vdpool->fillInputKComboBox( main_widget->input_combo_box );
    vdpool->fillStandardKComboBox( main_widget->standard_combo_box );
}


void MainWindow::deviceUnregistered( const QString& )
{
    vdpool->fillDeviceKComboBox( main_widget->device_combo_box );
    vdpool->fillInputKComboBox( main_widget->input_combo_box );
    vdpool->fillStandardKComboBox( main_widget->standard_combo_box );
}

void MainWindow::takePhoto( bool )
{
    QPixmap captured_image = QPixmap::fromImage( captured_frame );

    KUrl image_url( QDir::currentPath() + '/' + "videocapturetestapp_image.png" );
    QStringList mime_types = KImageIO::mimeTypes( KImageIO::Writing );
    KFileDialog file_dilaog( image_url, mime_types.join( " " ), this );

    file_dilaog.setOperationMode( KFileDialog::Saving );
    file_dilaog.setCaption( i18n( "Save As" ) );

    KImageFilePreview *image_preview = new KImageFilePreview( &file_dilaog );
    file_dilaog.setPreviewWidget( image_preview );

    if ( !file_dilaog.exec() ) {
        return;
    }

    image_url = file_dilaog.selectedUrl();
    if ( !image_url.isValid() ) {
        return;
    }

    saveImage( captured_image, image_url );

}


void MainWindow::saveImage( const QPixmap &image, const KUrl &image_url)
{
    if( KIO::NetAccess::exists( image_url, KIO::NetAccess::DestinationSide, this ) ) {
        const QString title = i18n( "File Exists" );
        const QString text = i18n( "<qt>Do you really want to overwrite <b>%1</b>?</qt>", image_url.prettyUrl() );
        if( KMessageBox::Continue != KMessageBox::warningContinueCancel( this, text, title, KGuiItem( i18n( "Overwrite" ) ) ) ) {
            return;
        }
    }

    QByteArray type = "PNG";
    QString mime_type = KMimeType::findByUrl( image_url.url(), 0, image_url.isLocalFile(), true )->name();
    QStringList types = KImageIO::typeForMime( mime_type );
    if( !types.isEmpty() ) {
        type = types.first().toLatin1();
    }

    bool ok = false;

    if( image_url.isLocalFile() ) {
        bool supported = false;
        QByteArray format;
        foreach( format, QImageWriter::supportedImageFormats() ) {
            if( format.toLower() == type.toLower() ) {
                supported = true;
            }
        }

        if( supported && image.save( image_url.path(), type ) ) {
            ok = true;
        }
    }
    else {
        KTemporaryFile temp_file;
        if( temp_file.open() ) {
            if( image.save( &temp_file, type ) ) {
                ok = KIO::NetAccess::upload( temp_file.fileName(), image_url, this );
            }
        }
    }

    QApplication::restoreOverrideCursor();

    if( !ok ) {
        QString caption = i18n( "Unable to Save Image" );
        QString text = i18n( "videocapturetestapp was unable to save the image to\n%1.", image_url.prettyUrl() );
        KMessageBox::error( this, text, caption );
    }
}
