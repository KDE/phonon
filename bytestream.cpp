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
#include "bytestream.h"
#include "ifaces/bytestream.h"
#include "factory.h"

namespace Phonon
{
class ByteStream::Private
{
	public:
		Private()
			: aboutToFinishTime( 0 )
			, streamSize( -1 )
			, streamSeekable( false )
		{ }

		long aboutToFinishTime;
		long streamSize;
		bool streamSeekable;
};

PHONON_HEIR_IMPL( ByteStream, AbstractMediaProducer )

long ByteStream::totalTime() const
{
	return m_iface ? m_iface->totalTime() : -1;
}

long ByteStream::remainingTime() const
{
	return m_iface ? m_iface->remainingTime() : -1;
}

long ByteStream::aboutToFinishTime() const
{
	return m_iface ? m_iface->aboutToFinishTime() : d->aboutToFinishTime;
}

long ByteStream::streamSize() const
{
	return m_iface ? m_iface->streamSize() : d->streamSize;
}

bool ByteStream::streamSeekable() const
{
	return m_iface ? m_iface->streamSeekable() : d->streamSeekable;
}

void ByteStream::setStreamSeekable( bool seekable )
{
	if( m_iface )
		m_iface->setStreamSeekable( seekable );
	else
		d->streamSeekable = seekable;
}

void ByteStream::writeData( const QByteArray& data )
{
	if( iface() )
		m_iface->writeData( data );
}

void ByteStream::setStreamSize( long streamSize )
{
	if( m_iface )
		m_iface->setStreamSize( streamSize );
	else
		d->streamSize = streamSize;
}

void ByteStream::endOfData()
{
	if( iface() )
		m_iface->endOfData();
}

void ByteStream::setAboutToFinishTime( long newAboutToFinishTime )
{
	if( m_iface )
		m_iface->setAboutToFinishTime( newAboutToFinishTime );
	else
		d->aboutToFinishTime = newAboutToFinishTime;
}

bool ByteStream::aboutToDeleteIface()
{
	if( m_iface )
	{
		d->aboutToFinishTime = m_iface->aboutToFinishTime();
		d->streamSize = m_iface->streamSize();
		d->streamSeekable = m_iface->streamSeekable();
	}
	return AbstractMediaProducer::aboutToDeleteIface();
}

void ByteStream::setupIface( Ifaces::ByteStream* iface )
{
	AbstractMediaProducer::setupIface( iface );

	m_iface = iface;
	if( !m_iface )
		return;

	connect( m_iface->qobject(), SIGNAL( finished() ), SIGNAL( finished() ) );
	connect( m_iface->qobject(), SIGNAL( aboutToFinish( long ) ), SIGNAL( aboutToFinish( long ) ) );
	connect( m_iface->qobject(), SIGNAL( length( long ) ), SIGNAL( length( long ) ) );
	connect( m_iface->qobject(), SIGNAL( needData() ), SIGNAL( needData() ) );
	connect( m_iface->qobject(), SIGNAL( enoughData() ), SIGNAL( enoughData() ) );
	connect( m_iface->qobject(), SIGNAL( seekStream( long ) ), SIGNAL( seekStream( long ) ) );

	m_iface->setAboutToFinishTime( d->aboutToFinishTime );
}

} //namespace Phonon

#include "bytestream.moc"

// vim: sw=4 ts=4 tw=80 noet
