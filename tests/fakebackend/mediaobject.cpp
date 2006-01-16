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

namespace Phonon
{
namespace Fake
{
MediaObject::MediaObject( QObject* parent )
	: AbstractMediaProducer( parent )
	, m_aboutToFinishTimer( new QTimer( this ) )
	, m_finishTimer( new QTimer( this ) )
{
	connect( m_aboutToFinishTimer, SIGNAL( timeout() ), SLOT( aboutToFinishTimeout() ) );
	connect( m_finishTimer, SIGNAL( timeout() ), SIGNAL( finished() ) );
	connect( m_finishTimer, SIGNAL( timeout() ), SLOT( stop() ) );
}

MediaObject::~MediaObject()
{
}

KURL MediaObject::url() const
{
	return m_url;
}

long MediaObject::totalTime() const
{
	return 1000*60*3; // 3 minutes
}

long MediaObject::aboutToFinishTime() const
{
	return m_aboutToFinishTime;
}

void MediaObject::setUrl( const KURL& url )
{
	stop();
	m_url = url;
	emit length( totalTime() );
}

long MediaObject::setAboutToFinishTime( long newAboutToFinishTime )
{
	m_aboutToFinishTime = newAboutToFinishTime;
	m_aboutToFinishTimer->setInterval( remainingTime() - m_aboutToFinishTime );
	return m_aboutToFinishTimer->interval();
}

void MediaObject::play()
{
	m_aboutToFinishTimer->setInterval( remainingTime() - m_aboutToFinishTime );
	m_finishTimer->setInterval( remainingTime() );
	AbstractMediaProducer::play();
	m_aboutToFinishTimer->start();
	m_finishTimer->start();
}

void MediaObject::pause()
{
	AbstractMediaProducer::pause();
	m_aboutToFinishTimer->stop();
	m_finishTimer->stop();
}

void MediaObject::stop()
{
	AbstractMediaProducer::stop();
	m_aboutToFinishTimer->stop();
	m_finishTimer->stop();
}

void MediaObject::seek( long time )
{
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
