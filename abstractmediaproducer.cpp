/*  This file is part of the KDE project
    Copyright (C) 2005-2006 Matthias Kretz <kretz@kde.org>

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

#include "abstractmediaproducer.h"
#include "abstractmediaproducer_p.h"
#include "ifaces/abstractmediaproducer.h"
#include "factory.h"

#include "videopath.h"
#include "audiopath.h"
#include "state.h"

#include <QTimer>

#include <kdebug.h>

namespace Phonon
{
PHONON_ABSTRACTBASE_IMPL( AbstractMediaProducer )

bool AbstractMediaProducer::addVideoPath( VideoPath* videoPath )
{
	Q_D( AbstractMediaProducer );
	if( d->videoPaths.contains( videoPath ) )
		return false;

	if( iface() && d->iface()->addVideoPath( videoPath->iface() ) )
	{
		connect( videoPath, SIGNAL( destroyed( Base* ) ), SLOT( videoPathDestroyed( Base* ) ) );
		d->videoPaths.append( videoPath );
		return true;
	}
	return false;
}

bool AbstractMediaProducer::addAudioPath( AudioPath* audioPath )
{
	Q_D( AbstractMediaProducer );
	if( d->audioPaths.contains( audioPath ) )
		return false;

	if( iface() && d->iface()->addAudioPath( audioPath->iface() ) )
	{
		connect( audioPath, SIGNAL( destroyed( Base* ) ), SLOT( audioPathDestroyed( Base* ) ) );
		d->audioPaths.append( audioPath );
		return true;
	}
	return false;
}

State AbstractMediaProducer::state() const
{
	Q_D( const AbstractMediaProducer );
	return d->iface() ? d->iface()->state() : d->state;
}

bool AbstractMediaProducer::hasVideo() const
{
	Q_D( const AbstractMediaProducer );
	return d->iface() ? d->iface()->hasVideo() : false;
}

bool AbstractMediaProducer::seekable() const
{
	Q_D( const AbstractMediaProducer );
	return d->iface() ? d->iface()->seekable() : false;
}

long AbstractMediaProducer::currentTime() const
{
	Q_D( const AbstractMediaProducer );
	return d->iface() ? d->iface()->currentTime() : d->currentTime;
}

long AbstractMediaProducer::tickInterval() const
{
	Q_D( const AbstractMediaProducer );
	return d->iface() ? d->iface()->tickInterval() : d->tickInterval;
}

void AbstractMediaProducer::setTickInterval( long newTickInterval )
{
	Q_D( AbstractMediaProducer );
	if( d->iface() )
		d->tickInterval = d->iface()->setTickInterval( newTickInterval );
	else
		d->tickInterval = newTickInterval;
}

const QList<VideoPath*>& AbstractMediaProducer::videoPaths() const
{
	Q_D( const AbstractMediaProducer );
	return d->videoPaths;
}

const QList<AudioPath*>& AbstractMediaProducer::audioPaths() const
{
	Q_D( const AbstractMediaProducer );
	return d->audioPaths;
}

void AbstractMediaProducer::play()
{
	Q_D( AbstractMediaProducer );
	if( iface() )
		d->iface()->play();
}

void AbstractMediaProducer::pause()
{
	Q_D( AbstractMediaProducer );
	if( iface() )
		d->iface()->pause();
}

void AbstractMediaProducer::stop()
{
	Q_D( AbstractMediaProducer );
	if( iface() )
		d->iface()->stop();
}

void AbstractMediaProducer::seek( long time )
{
	Q_D( AbstractMediaProducer );
	if( iface() )
		d->iface()->seek( time );
}

bool AbstractMediaProducerPrivate::aboutToDeleteIface()
{
	//kdDebug( 600 ) << k_funcinfo << endl;
	if( iface() )
	{
		state = iface()->state();
		currentTime = iface()->currentTime();
		tickInterval = iface()->tickInterval();
	}
	return true;
}

void AbstractMediaProducer::setupIface()
{
	Q_D( AbstractMediaProducer );
	Q_ASSERT( d->iface() );
	//kdDebug( 600 ) << k_funcinfo << endl;

	connect( d->iface()->qobject(), SIGNAL( stateChanged( Phonon::State, Phonon::State ) ), SIGNAL( stateChanged( Phonon::State, Phonon::State ) ) );
	connect( d->iface()->qobject(), SIGNAL( tick( long ) ), SIGNAL( tick( long ) ) );

	// set up attributes
	d->iface()->setTickInterval( d->tickInterval );

	foreach( AudioPath* a, d->audioPaths )
		d->iface()->addAudioPath( a->iface() );
	foreach( VideoPath* v, d->videoPaths )
		d->iface()->addVideoPath( v->iface() );

	switch( d->state )
	{
		case LoadingState:
		case StoppedState:
		case ErrorState:
			break;
		case PlayingState:
		case BufferingState:
			QTimer::singleShot( 0, this, SLOT( resumePlay() ) );
			break;
		case PausedState:
			QTimer::singleShot( 0, this, SLOT( resumePause() ) );
			break;
	}
	d->state = d->iface()->state();
}

void AbstractMediaProducer::resumePlay()
{
	Q_D( AbstractMediaProducer );
	d->iface()->play();
	if( d->currentTime > 0 )
		d->iface()->seek( d->currentTime );
}

void AbstractMediaProducer::resumePause()
{
	Q_D( AbstractMediaProducer );
	d->iface()->play();
	if( d->currentTime > 0 )
		d->iface()->seek( d->currentTime );
	d->iface()->pause();
}

void AbstractMediaProducerPrivate::audioPathDestroyed( Base* o )
{
	Q_ASSERT( o );
	AudioPath* audioPath = static_cast<AudioPath*>( o );
	if( iface() )
	{
		iface()->removeAudioPath( audioPath->iface() );
		audioPaths.removeAll( audioPath );
	}
}

void AbstractMediaProducerPrivate::videoPathDestroyed( Base* o )
{
	Q_ASSERT( o );
	VideoPath* videoPath = static_cast<VideoPath*>( o );
	if( iface() )
	{
		iface()->removeVideoPath( videoPath->iface() );
		videoPaths.removeAll( videoPath );
	}
}

} //namespace Phonon

#include "abstractmediaproducer.moc"

// vim: sw=4 ts=4 tw=80 noet
