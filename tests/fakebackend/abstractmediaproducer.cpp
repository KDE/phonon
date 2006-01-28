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
	, m_fakeTime( 0 )
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
	return m_fakeTime;
}

long AbstractMediaProducer::tickInterval() const
{
	kdDebug() << k_funcinfo << endl;
	return m_tickTimer->interval();
}

long AbstractMediaProducer::setTickInterval( long newTickInterval )
{
	kdDebug() << k_funcinfo << endl;
	m_tickTimer->setInterval( newTickInterval );
	return m_tickTimer->interval();
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
	m_fakeTime = 0;
	setState( Phonon::StoppedState );
}

void AbstractMediaProducer::seek( long time )
{
	kdDebug() << k_funcinfo << endl;
	if( seekable() )
		m_fakeTime = time;
}

void AbstractMediaProducer::setState( State newstate )
{
	if( newstate == m_state )
		return;
	State oldstate = m_state;
	m_state = newstate;
	kdDebug() << "emit stateChanged( " << newstate << ", " << oldstate << " )" << endl;
	emit stateChanged( newstate, oldstate );
}

void AbstractMediaProducer::emitTick()
{
	m_fakeTime += m_tickTimer->interval();
	//kdDebug() << "emit tick( " << currentTime() << " )" << endl;
	emit tick( currentTime() );
}

}}
#include "abstractmediaproducer.moc"
// vim: sw=4 ts=4 noet
