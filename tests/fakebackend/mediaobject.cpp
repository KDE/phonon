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

#include "mediaobject.h"
#include <QTimer>
#include <kdebug.h>

namespace Phonon
{
namespace Fake
{
MediaObject::MediaObject( QObject* parent )
	: AbstractMediaProducer( parent )
	, m_aboutToFinishTimer( new QTimer( this ) )
	, m_finishTimer( new QTimer( this ) )
{
	kdDebug() << k_funcinfo << endl;
	connect( m_aboutToFinishTimer, SIGNAL( timeout() ), SLOT( aboutToFinishTimeout() ) );
	connect( m_finishTimer, SIGNAL( timeout() ), SIGNAL( finished() ) );
	connect( m_finishTimer, SIGNAL( timeout() ), SLOT( stop() ) );
}

MediaObject::~MediaObject()
{
	kdDebug() << k_funcinfo << endl;
}

KURL MediaObject::url() const
{
	kdDebug() << k_funcinfo << endl;
	return m_url;
}

long MediaObject::totalTime() const
{
	kdDebug() << k_funcinfo << endl;
	return 1000*60*3; // 3 minutes
}

long MediaObject::aboutToFinishTime() const
{
	kdDebug() << k_funcinfo << endl;
	return m_aboutToFinishTime;
}

void MediaObject::setUrl( const KURL& url )
{
	kdDebug() << k_funcinfo << endl;
	stop();
	m_url = url;
	emit length( totalTime() );
}

void MediaObject::setAboutToFinishTime( long newAboutToFinishTime )
{
	kdDebug() << k_funcinfo << endl;
	m_aboutToFinishTime = newAboutToFinishTime;
	m_aboutToFinishTimer->setInterval( remainingTime() - m_aboutToFinishTime );
}

void MediaObject::play()
{
	kdDebug() << k_funcinfo << endl;
	m_aboutToFinishTimer->setInterval( remainingTime() - m_aboutToFinishTime );
	m_finishTimer->setInterval( remainingTime() );
	AbstractMediaProducer::play();
	m_aboutToFinishTimer->start();
	m_finishTimer->start();
}

void MediaObject::pause()
{
	kdDebug() << k_funcinfo << endl;
	if( state() == PlayingState || state() == BufferingState )
	{
		AbstractMediaProducer::pause();
		m_aboutToFinishTimer->stop();
		m_finishTimer->stop();
	}
}

void MediaObject::stop()
{
	kdDebug() << k_funcinfo << endl;
	AbstractMediaProducer::stop();
	m_aboutToFinishTimer->stop();
	m_finishTimer->stop();
}

void MediaObject::seek( long time )
{
	kdDebug() << k_funcinfo << endl;
	AbstractMediaProducer::seek( time );
	m_aboutToFinishTimer->setInterval( remainingTime() - m_aboutToFinishTime );
	m_finishTimer->setInterval( remainingTime() );
}

void MediaObject::aboutToFinishTimeout()
{
	emit aboutToFinish( remainingTime() );
}

}}

#include "mediaobject.moc"
// vim: sw=4 ts=4 noet
