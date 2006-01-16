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

#include "volumefader.h"
#include "../audiooutput.h"
#include <QSlider>
#include <QHBoxLayout>

namespace Kdem2m
{
namespace Ui
{

class VolumeFader::Private
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

VolumeFader::VolumeFader( QWidget* parent )
	: QWidget( parent )
	, d( new Private )
{
	QHBoxLayout* layout = new QHBoxLayout( this );
	d->slider = new QSlider( Qt::Vertical, this );
	d->slider->setRange( 0, 150 );
	layout->addWidget( d->slider );
}

VolumeFader::~VolumeFader()
{
	delete d;
}

void VolumeFader::setOrientation( Qt::Orientation o )
{
	d->slider->setOrientation( o );
}

void VolumeFader::setAudioOutput( AudioOutput* output )
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

void VolumeFader::sliderChanged( int value )
{
	d->ignoreVolumeChange = true;
	d->output->setVolume( ( static_cast<float>( value ) ) * 0.01 );
	d->ignoreVolumeChange = false;
}

void VolumeFader::volumeChanged( float value )
{
	if( !d->ignoreVolumeChange )
		d->slider->setValue( qRound( 100 * value ) );
}

void VolumeFader::outputDestroyed()
{
	d->output = 0;
	d->slider->setValue( 100 );
	d->slider->setEnabled( false );
}

}} // namespace Kdem2m::Ui

#include "volumefader.moc"
// vim: sw=4 ts=4 noet
