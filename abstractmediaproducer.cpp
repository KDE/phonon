/*  This file is part of the KDE project
    Copyright (C) 2005 Matthias Kretz <kretz@kde.org>

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
#include "ifaces/abstractmediaproducer.h"
#include "factory.h"

#include "videopath.h"
#include "audiopath.h"
#include "state.h"

#include <QTimer>

#include <kdebug.h>

namespace Phonon
{
class AbstractMediaProducer::Private
{
	public:
		Private()
			: state( Phonon::LoadingState )
			, currentTime( 0 )
			, tickInterval( 0 )
		{ }

		State state;
		long currentTime;
		long tickInterval;
		QList<VideoPath*> videoPaths;
		QList<AudioPath*> audioPaths;
};

PHONON_ABSTRACTBASE_IMPL( AbstractMediaProducer )

bool AbstractMediaProducer::addVideoPath( VideoPath* videoPath )
{
	d->videoPaths.append( videoPath );
	if( iface() )
		return m_iface->addVideoPath( videoPath->iface() );
	return false;
}

bool AbstractMediaProducer::addAudioPath( AudioPath* audioPath )
{
	d->audioPaths.append( audioPath );
	if( iface() )
		return m_iface->addAudioPath( audioPath->iface() );
	return false;
}

State AbstractMediaProducer::state() const
{
	return m_iface ? m_iface->state() : d->state;
}

bool AbstractMediaProducer::hasVideo() const
{
	return m_iface ? m_iface->hasVideo() : false;
}

bool AbstractMediaProducer::seekable() const
{
	return m_iface ? m_iface->seekable() : false;
}

long AbstractMediaProducer::currentTime() const
{
	return m_iface ? m_iface->currentTime() : d->currentTime;
}

long AbstractMediaProducer::tickInterval() const
{
	return m_iface ? m_iface->tickInterval() : d->tickInterval;
}

void AbstractMediaProducer::setTickInterval( long newTickInterval )
{
	if( m_iface )
		d->tickInterval = m_iface->setTickInterval( newTickInterval );
	else
		d->tickInterval = newTickInterval;
}

const QList<VideoPath*>& AbstractMediaProducer::videoPaths() const
{
	return d->videoPaths;
}

const QList<AudioPath*>& AbstractMediaProducer::audioPaths() const
{
	return d->audioPaths;
}

void AbstractMediaProducer::play()
{
	if( iface() )
		m_iface->play();
}

void AbstractMediaProducer::pause()
{
	if( iface() )
		m_iface->pause();
}

void AbstractMediaProducer::stop()
{
	if( iface() )
		m_iface->stop();
}

void AbstractMediaProducer::seek( long time )
{
	if( iface() )
		m_iface->seek( time );
}

bool AbstractMediaProducer::aboutToDeleteIface()
{
	kdDebug( 600 ) << k_funcinfo << endl;
	if( m_iface )
	{
		d->state = m_iface->state();
		d->currentTime = m_iface->currentTime();
		d->tickInterval = m_iface->tickInterval();
	}
	return true;
}

void AbstractMediaProducer::setupIface()
{
	kdDebug( 600 ) << k_funcinfo << endl;
	if( !m_iface )
		return;

	connect( m_iface->qobject(), SIGNAL( stateChanged( Phonon::State, Phonon::State ) ), SIGNAL( stateChanged( Phonon::State, Phonon::State ) ) );
	connect( m_iface->qobject(), SIGNAL( tick( long ) ), SIGNAL( tick( long ) ) );

	// set up attributes
	m_iface->setTickInterval( d->tickInterval );

	// FIXME: seeking probably only works in PlayingState
	if( d->currentTime > 0 )
		m_iface->seek( d->currentTime );

	foreach( AudioPath* a, d->audioPaths )
		m_iface->addAudioPath( a->iface() );
	foreach( VideoPath* v, d->videoPaths )
		m_iface->addVideoPath( v->iface() );

	switch( d->state )
	{
		case LoadingState:
		case StoppedState:
		case ErrorState:
			break;
		case PlayingState:
		case BufferingState:
			QTimer::singleShot( 0, this, SLOT( play() ) );
			break;
		case PausedState:
			// FIXME: Going from StoppedState or even LoadingState into
			// PausedState is not going to work.
			QTimer::singleShot( 0, this, SLOT( pause() ) );
			break;
	}
	d->state = m_iface->state();
}

} //namespace Phonon

#include "abstractmediaproducer.moc"

// vim: sw=4 ts=4 tw=80 noet
