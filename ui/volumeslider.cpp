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

#include "volumeslider.h"
#include "../audiooutput.h"
#include <QSlider>
#include <QHBoxLayout>

namespace Phonon
{
namespace Ui
{

class VolumeSlider::Private
{
	public:
		Private()
			: slider( 0 )
			, output( 0 )
			, ignoreVolumeChange( false )
		{
		}

		QSlider* slider;
		AudioOutput* output;
		bool ignoreVolumeChange;
};

VolumeSlider::VolumeSlider( QWidget* parent )
	: QWidget( parent )
	, d( new Private )
{
	QHBoxLayout* layout = new QHBoxLayout( this );
	d->slider = new QSlider( Qt::Vertical, this );
	d->slider->setRange( 0, 150 );
	layout->addWidget( d->slider );
}

VolumeSlider::~VolumeSlider()
{
	delete d;
}

void VolumeSlider::setOrientation( Qt::Orientation o )
{
	d->slider->setOrientation( o );
}

void VolumeSlider::setAudioOutput( AudioOutput* output )
{
	if( !output )
		return;

	d->output = output;
	d->slider->setValue( qRound( 100 * output->volume() ) );
	d->slider->setEnabled( true );
	connect( output, SIGNAL( destroyed() ), SLOT( outputDestroyed() ) );
	connect( d->slider, SIGNAL( valueChanged( int ) ), SLOT( sliderChanged( int ) ) );
	connect( output, SIGNAL( volumeChanged( float ) ), SLOT( volumeChanged( float ) ) );
}

void VolumeSlider::sliderChanged( int value )
{
	d->ignoreVolumeChange = true;
	d->output->setVolume( ( static_cast<float>( value ) ) * 0.01 );
	d->ignoreVolumeChange = false;
}

void VolumeSlider::volumeChanged( float value )
{
	if( !d->ignoreVolumeChange )
		d->slider->setValue( qRound( 100 * value ) );
}

void VolumeSlider::outputDestroyed()
{
	d->output = 0;
	d->slider->setValue( 100 );
	d->slider->setEnabled( false );
}

}} // namespace Phonon::Ui

#include "volumeslider.moc"
// vim: sw=4 ts=4 noet
