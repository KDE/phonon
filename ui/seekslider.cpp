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
#include "seekslider_p.h"
#include "../abstractmediaproducer.h"
#include <QSlider>
#include <QHBoxLayout>

namespace Phonon
{
namespace Ui
{

SeekSlider::SeekSlider( QWidget* parent )
	: QWidget( parent )
	, d_ptr( new SeekSliderPrivate )
{
	Q_D( SeekSlider );
	QHBoxLayout* layout = new QHBoxLayout( this );
	d->slider = new QSlider( Qt::Horizontal, this );
	d->slider->setEnabled( false );
	layout->addWidget( d->slider );
}

SeekSlider::SeekSlider( SeekSliderPrivate& _d, QWidget* parent )
	: QWidget( parent )
	, d_ptr( &_d )
{
	Q_D( SeekSlider );
	QHBoxLayout* layout = new QHBoxLayout( this );
	d->slider = new QSlider( Qt::Horizontal, this );
	d->slider->setEnabled( false );
	layout->addWidget( d->slider );
}

SeekSlider::~SeekSlider()
{
	delete d_ptr;
}

void SeekSlider::setMediaProducer( AbstractMediaProducer* media )
{
	if( !media )
		return;

	Q_D( SeekSlider );
	d->media = media;
	connect( media, SIGNAL( stateChanged( Phonon::State, Phonon::State ) ),
			SLOT( stateChanged( Phonon::State ) ) );
	connect( d->slider, SIGNAL( valueChanged( int ) ), SLOT( seek( int ) ) );
	connect( media, SIGNAL( length( long ) ), SLOT( length( long ) ) );
	connect( media, SIGNAL( tick( long ) ), SLOT( tick( long ) ) );
	stateChanged( media->state() );
}

void SeekSlider::seek( int msec )
{
	Q_D( SeekSlider );
	if( ! d->ticking && d->media )
		d->media->seek( msec );
}

void SeekSlider::tick( long msec )
{
	Q_D( SeekSlider );
	d->ticking = true;
	d->slider->setValue( msec );
	d->ticking = false;
}

void SeekSlider::length( long msec )
{
	Q_D( SeekSlider );
	d->slider->setRange( 0, msec );
}

void SeekSlider::stateChanged( State newstate )
{
	Q_D( SeekSlider );
	switch( newstate )
	{
		case Phonon::BufferingState:
		case Phonon::PlayingState:
			d->slider->setEnabled( true );
			break;
		case Phonon::PausedState:
		case Phonon::StoppedState:
		case Phonon::LoadingState:
		case Phonon::ErrorState:
			d->slider->setEnabled( false );
			break;
	}
}

void SeekSlider::mediaDestroyed()
{
	Q_D( SeekSlider );
	d->media = 0;
	d->slider->setEnabled( false );
}

}} // namespace Phonon::Ui

#include "seekslider.moc"
// vim: sw=4 ts=4 noet
