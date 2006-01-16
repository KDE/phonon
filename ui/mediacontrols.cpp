/*  This file is part of the KDE project
    Copyright (C) 2006 Matthias Kretz <kretz@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License version 2 as published by the Free Software Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.

*/

#include "mediacontrols.h"
#include "../abstractmediaproducer.h"
#include "../audiooutput.h"
#include <kglobal.h>
#include <kiconloader.h>
#include <klocale.h>
#include <QHBoxLayout>
#include <QToolButton>
#include "seekslider.h"
#include "volumefader.h"

namespace Phonon
{
namespace Ui
{

class MediaControls::Private
{
	public:
		Private()
			: layout( 0 )
			, playButton( 0 )
			, pauseButton( 0 )
			, stopButton( 0 )
			, loopButton( 0 )
			, seekSlider( 0 )
			, volumeFader( 0 )
			, media( 0 )
		{
		}

		QHBoxLayout* layout;
		QToolButton* playButton;
		QToolButton* pauseButton;
		QToolButton* stopButton;
		QToolButton* loopButton;
		SeekSlider*  seekSlider;
		VolumeFader* volumeFader;
		AbstractMediaProducer* media;
};

MediaControls::MediaControls( QWidget* parent )
	: QWidget( parent )
	, d( new Private )
{
	d->playButton = new QToolButton( this );
	d->playButton->setIcon( KGlobal::iconLoader()->loadIconSet( "player_play", KIcon::Small ) );
	d->playButton->setText( i18n( "&Play" ) );

	d->pauseButton = new QToolButton( this );
	d->pauseButton->setIcon( KGlobal::iconLoader()->loadIconSet( "player_pause", KIcon::Small ) );
	d->pauseButton->setText( i18n( "&Pause" ) );
	d->pauseButton->setEnabled( false );

	d->stopButton = new QToolButton( this );
	d->stopButton->setIcon( KGlobal::iconLoader()->loadIconSet( "player_stop", KIcon::Small ) );
	d->stopButton->setText( i18n( "&Stop" ) );

	d->seekSlider = new SeekSlider( this );
	d->seekSlider->setEnabled( false );

	d->volumeFader = new VolumeFader( this );
	d->volumeFader->setOrientation( Qt::Horizontal );
	d->volumeFader->hide();

	d->layout = new QHBoxLayout( this );
	d->layout->addWidget( d->playButton );
	d->layout->addWidget( d->pauseButton );
	d->layout->addWidget( d->stopButton );
	d->layout->addWidget( d->seekSlider );
	d->layout->addWidget( d->volumeFader );
	d->layout->addStretch();
}

MediaControls::~MediaControls()
{
	delete d;
}

bool MediaControls::isSeekSliderVisible() const
{
	return d->seekSlider->isVisible();
}

bool MediaControls::isVolumeControlVisible() const
{
	return d->volumeFader->isVisible();
}

bool MediaControls::isLoopControlVisible() const
{
	return d->loopButton->isVisible();
}

void MediaControls::setMediaProducer( AbstractMediaProducer* media )
{
	if( !media )
		return;

	d->media = media;
	connect( d->media, SIGNAL( destroyed() ), SLOT( mediaDestroyed() ) );
	connect( d->media, SIGNAL( stateChanged( Phonon::State, Phonon::State ) ),
		   	SLOT( stateChanged( Phonon::State, Phonon::State ) ) );
	connect( d->playButton, SIGNAL( clicked() ), d->media, SLOT( play() ) );
	connect( d->pauseButton, SIGNAL( clicked() ), d->media, SLOT( pause() ) );
	connect( d->stopButton, SIGNAL( clicked() ), d->media, SLOT( stop() ) );

	d->seekSlider->setMediaProducer( d->media );
}

void MediaControls::setAudioOutput( AudioOutput* audioOutput )
{
	d->volumeFader->setAudioOutput( audioOutput );
}

void MediaControls::setSeekSliderVisible( bool vis )
{
	d->seekSlider->setVisible( vis );
}

void MediaControls::setVolumeControlVisible( bool vis )
{
	d->volumeFader->setVisible( vis );
}

void MediaControls::setLoopControlVisible( bool vis )
{
	d->loopButton->setVisible( vis );
}

void MediaControls::stateChanged( State newstate, State )
{
	switch( newstate )
	{
		case Phonon::LoadingState:
		case Phonon::PausedState:
		case Phonon::StoppedState:
			d->playButton->setEnabled( true );
			d->pauseButton->setEnabled( false );
			break;
		case Phonon::BufferingState:
		case Phonon::PlayingState:
			d->playButton->setEnabled( false );
			d->pauseButton->setEnabled( true );
			break;
		case Phonon::ErrorState:
			return;
	}
}

void MediaControls::mediaDestroyed()
{
	d->media = 0;
}

}} // namespace Phonon::Ui

#include "mediacontrols.moc"
// vim: sw=4 ts=4 noet
