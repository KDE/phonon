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

#include "seekslider.h"
#include "../abstractmediaproducer.h"
#include <QSlider>
#include <QHBoxLayout>

namespace Kdem2m
{
namespace Ui
{

class SeekSlider::Private
{
	public:
		Private()
			: slider( 0 )
			, media( 0 )
			, ticking( false )
		{
		}

		QSlider* slider;
		AbstractMediaProducer* media;
		bool ticking;
};

SeekSlider::SeekSlider( QWidget* parent )
	: QWidget( parent )
	, d( new Private )
{
	QHBoxLayout* layout = new QHBoxLayout( this );
	d->slider = new QSlider( Qt::Horizontal, this );
	d->slider->setEnabled( false );
	layout->addWidget( d->slider );
}

SeekSlider::~SeekSlider()
{
	delete d;
}

void SeekSlider::setMediaProducer( AbstractMediaProducer* media )
{
	if( !media )
		return;

	d->media = media;
	connect( media, SIGNAL( stateChanged( Kdem2m::State, Kdem2m::State ) ),
			SLOT( stateChanged( Kdem2m::State ) ) );
	connect( d->slider, SIGNAL( valueChanged( int ) ), SLOT( seek( int ) ) );
	connect( media, SIGNAL( length( long ) ), SLOT( length( long ) ) );
	connect( media, SIGNAL( tick( long ) ), SLOT( tick( long ) ) );
	stateChanged( media->state() );
}

void SeekSlider::seek( int msec )
{
	if( ! d->ticking && d->media )
		d->media->seek( msec );
}

void SeekSlider::tick( long msec )
{
	d->ticking = true;
	d->slider->setValue( msec );
	d->ticking = false;
}

void SeekSlider::length( long msec )
{
	d->slider->setRange( 0, msec );
}

void SeekSlider::stateChanged( State newstate )
{
	switch( newstate )
	{
		case Kdem2m::BufferingState:
		case Kdem2m::PlayingState:
			d->slider->setEnabled( true );
		case Kdem2m::PausedState:
		case Kdem2m::StoppedState:
		case Kdem2m::LoadingState:
		case Kdem2m::ErrorState:
			d->slider->setEnabled( false );
	}
}

void SeekSlider::mediaDestroyed()
{
	d->media = 0;
	d->slider->setEnabled( false );
}

}} // namespace Kdem2m::Ui

#include "seekslider.moc"
// vim: sw=4 ts=4 noet
