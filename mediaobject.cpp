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
#include "mediaobject.h"
#include "ifaces/mediaobject.h"
#include "factory.h"

#include <kdebug.h>

namespace Phonon
{
class MediaObject::Private
{
	public:
		Private()
			: aboutToFinishTime( 0 )
		{ }

		KURL url;
		long aboutToFinishTime;
};

PHONON_HEIR_IMPL( MediaObject, AbstractMediaProducer )

KURL MediaObject::url() const
{
	return m_iface ? m_iface->url() : d->url;
}

long MediaObject::totalTime() const
{
	return m_iface ? m_iface->totalTime() : -1;
}

long MediaObject::remainingTime() const
{
	return m_iface ? m_iface->remainingTime() : -1;
}

long MediaObject::aboutToFinishTime() const
{
	return m_iface ? m_iface->aboutToFinishTime() : d->aboutToFinishTime;
}

void MediaObject::setUrl( const KURL& url )
{
	if( iface() )
		m_iface->setUrl( url );
	else
		d->url = url;
}

void MediaObject::setAboutToFinishTime( long newAboutToFinishTime )
{
	kdDebug( 600 ) << k_funcinfo << endl;
	if( m_iface )
		m_iface->setAboutToFinishTime( newAboutToFinishTime );
	else
		d->aboutToFinishTime = newAboutToFinishTime;
}

bool MediaObject::aboutToDeleteIface()
{
	kdDebug( 600 ) << k_funcinfo << endl;
	if( m_iface )
		d->aboutToFinishTime = m_iface->aboutToFinishTime();
	return AbstractMediaProducer::aboutToDeleteIface();
}

void MediaObject::setupIface( Ifaces::MediaObject* iface )
{
	kdDebug( 600 ) << k_funcinfo << endl;
	AbstractMediaProducer::setupIface( iface );

	m_iface = iface;
	if( !m_iface )
		return;

	connect( m_iface->qobject(), SIGNAL( finished() ), SIGNAL( finished() ) );
	connect( m_iface->qobject(), SIGNAL( aboutToFinish( long ) ), SIGNAL( aboutToFinish( long ) ) );
	connect( m_iface->qobject(), SIGNAL( length( long ) ), SIGNAL( length( long ) ) );

	// set up attributes
	m_iface->setUrl( d->url );
	if( state() == Phonon::ErrorState )
	{
		deleteIface();
		//TODO: at this point MediaObject should try to use an Ifaces::ByteStream
		//instead and send the data it receives from a KIO Job via writeBuffer.
		//This essentially makes all media frameworks read data via KIO...
		return;
	}
	m_iface->setAboutToFinishTime( d->aboutToFinishTime );
}

} //namespace Phonon

#include "mediaobject.moc"

// vim: sw=4 ts=4 tw=80 noet
