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

namespace Kdem2m
{
namespace Fake
{
AbstractMediaProducer::AbstractMediaProducer( QObject* parent )
	: QObject( parent )
	, m_state( Kdem2m::LoadingState )
	, m_tickTimer( new QTimer( this ) )
	, m_fakeTime( 0 )
{
	connect( m_tickTimer, SIGNAL( timeout() ), SLOT( emitTick() ) );
}

AbstractMediaProducer::~AbstractMediaProducer()
{
}

bool AbstractMediaProducer::addVideoPath( Ifaces::VideoPath* videoPath )
{
	Q_ASSERT( videoPath );
	VideoPath* vp = qobject_cast<VideoPath*>( videoPath->qobject() );
	Q_ASSERT( vp );
	return true;
}

bool AbstractMediaProducer::addAudioPath( Ifaces::AudioPath* audioPath )
{
	Q_ASSERT( audioPath );
	AudioPath* ap = qobject_cast<AudioPath*>( audioPath->qobject() );
	Q_ASSERT( ap );
	return true;
}

State AbstractMediaProducer::state() const
{
	return m_state;
}

bool AbstractMediaProducer::hasVideo() const
{
	return false;
}

bool AbstractMediaProducer::seekable() const
{
	return true;
}

long AbstractMediaProducer::currentTime() const
{
	return m_fakeTime;
}

long AbstractMediaProducer::tickInterval() const
{
	return m_tickTimer->interval();
}

long AbstractMediaProducer::setTickInterval( long newTickInterval )
{
	m_tickTimer->setInterval( newTickInterval );
	return m_tickTimer->interval();
}

void AbstractMediaProducer::play()
{
	m_tickTimer->start();
	setState( Kdem2m::PlayingState );
}

void AbstractMediaProducer::pause()
{
	m_tickTimer->stop();
	setState( Kdem2m::PausedState );
}

void AbstractMediaProducer::stop()
{
	m_tickTimer->stop();
	m_fakeTime = 0;
	setState( Kdem2m::StoppedState );
}

void AbstractMediaProducer::seek( long time )
{
	if( seekable() )
		m_fakeTime = time;
}

void AbstractMediaProducer::setState( State newstate )
{
	State oldstate = m_state;
	m_state = newstate;
	emit stateChanged( newstate, oldstate );
}

void AbstractMediaProducer::emitTick()
{
	m_fakeTime += m_tickTimer->interval();
	emit tick( currentTime() );
}

}}
#include "abstractmediaproducer.moc"
// vim: sw=4 ts=4 noet
