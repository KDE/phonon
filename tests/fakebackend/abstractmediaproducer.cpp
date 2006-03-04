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

#include "abstractmediaproducer.h"
#include <QTimer>
#include "videopath.h"
#include "audiopath.h"
#include <kdebug.h>

namespace Phonon
{
namespace Fake
{
AbstractMediaProducer::AbstractMediaProducer( QObject* parent )
	: QObject( parent )
	, m_state( Phonon::LoadingState )
	, m_tickTimer( new QTimer( this ) )
{
	kdDebug() << k_funcinfo << endl;
	connect( m_tickTimer, SIGNAL( timeout() ), SLOT( emitTick() ) );
}

AbstractMediaProducer::~AbstractMediaProducer()
{
	kdDebug() << k_funcinfo << endl;
}

bool AbstractMediaProducer::addVideoPath( Ifaces::VideoPath* videoPath )
{
	kdDebug() << k_funcinfo << endl;
	Q_ASSERT( videoPath );
	VideoPath* vp = qobject_cast<VideoPath*>( videoPath->qobject() );
	Q_ASSERT( vp );
	return true;
}

bool AbstractMediaProducer::addAudioPath( Ifaces::AudioPath* audioPath )
{
	kdDebug() << k_funcinfo << endl;
	Q_ASSERT( audioPath );
	AudioPath* ap = qobject_cast<AudioPath*>( audioPath->qobject() );
	Q_ASSERT( ap );
	return true;
}

void AbstractMediaProducer::removeVideoPath( Ifaces::VideoPath* videoPath )
{
	Q_ASSERT( videoPath );
	VideoPath* vp = qobject_cast<VideoPath*>( videoPath->qobject() );
	Q_ASSERT( vp );
}

void AbstractMediaProducer::removeAudioPath( Ifaces::AudioPath* audioPath )
{
	Q_ASSERT( audioPath );
	AudioPath* ap = qobject_cast<AudioPath*>( audioPath->qobject() );
	Q_ASSERT( ap );
}

State AbstractMediaProducer::state() const
{
	kdDebug() << k_funcinfo << endl;
	return m_state;
}

bool AbstractMediaProducer::hasVideo() const
{
	kdDebug() << k_funcinfo << endl;
	return false;
}

bool AbstractMediaProducer::seekable() const
{
	kdDebug() << k_funcinfo << endl;
	return true;
}

long AbstractMediaProducer::currentTime() const
{
	//kdDebug() << k_funcinfo << endl;
	switch( state() )
	{
		case Phonon::PausedState:
		case Phonon::BufferingState:
			return m_startTime.msecsTo( m_pauseTime );
		case Phonon::PlayingState:
			return m_startTime.elapsed();
		case Phonon::StoppedState:
		case Phonon::LoadingState:
			return 0;
		case Phonon::ErrorState:
			break;
	}
	return -1;
}

long AbstractMediaProducer::tickInterval() const
{
	kdDebug() << k_funcinfo << endl;
	return m_tickInterval;
}

long AbstractMediaProducer::setTickInterval( long newTickInterval )
{
	kdDebug() << k_funcinfo << endl;
	m_tickInterval = newTickInterval;
	if( m_tickInterval <= 0 )
		m_tickTimer->setInterval( 50 );
	else
		m_tickTimer->setInterval( newTickInterval );
	return m_tickInterval;
}

void AbstractMediaProducer::play()
{
	kdDebug() << k_funcinfo << endl;
	m_tickTimer->start();
	setState( Phonon::PlayingState );
}

void AbstractMediaProducer::pause()
{
	kdDebug() << k_funcinfo << endl;
	m_tickTimer->stop();
	setState( Phonon::PausedState );
}

void AbstractMediaProducer::stop()
{
	kdDebug() << k_funcinfo << endl;
	m_tickTimer->stop();
	setState( Phonon::StoppedState );
}

void AbstractMediaProducer::seek( long time )
{
	kdDebug() << k_funcinfo << endl;
	if( seekable() )
	{
		switch( state() )
		{
			case Phonon::PausedState:
			case Phonon::BufferingState:
				m_startTime = m_pauseTime;
				break;
			case Phonon::PlayingState:
				m_startTime = QTime::currentTime();
				break;
			case Phonon::StoppedState:
			case Phonon::ErrorState:
			case Phonon::LoadingState:
				return; // cannot seek
		}
		m_startTime = m_startTime.addMSecs( -time );
	}
}

void AbstractMediaProducer::setState( State newstate )
{
	if( newstate == m_state )
		return;
	State oldstate = m_state;
	m_state = newstate;
	switch( newstate )
	{
		case Phonon::PausedState:
		case Phonon::BufferingState:
			m_pauseTime.start();
			break;
		case Phonon::PlayingState:
			if( oldstate == Phonon::PausedState || oldstate == Phonon::BufferingState )
				m_startTime = m_startTime.addMSecs( m_pauseTime.elapsed() );
			else
				m_startTime.start();
			break;
		case Phonon::StoppedState:
		case Phonon::ErrorState:
		case Phonon::LoadingState:
			break;
	}
	kdDebug() << "emit stateChanged( " << newstate << ", " << oldstate << " )" << endl;
	emit stateChanged( newstate, oldstate );
}

void AbstractMediaProducer::emitTick()
{
	//kdDebug() << "emit tick( " << currentTime() << " )" << endl;
	if( m_tickInterval > 0 )
		emit tick( currentTime() );
}

}}
#include "abstractmediaproducer.moc"
// vim: sw=4 ts=4 noet
