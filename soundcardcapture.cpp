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
#include "soundcardcapture.h"
#include "ifaces/soundcardcapture.h"
#include "factory.h"
#include "capturesource.h"
#include "ifaces/backend.h"

namespace Phonon
{
class SoundcardCapture::Private
{
	public:
		Private()
		{ }

		CaptureSource source;
};

PHONON_HEIR_IMPL( SoundcardCapture, AbstractMediaProducer )

const CaptureSource& SoundcardCapture::captureSource() const
{
	if( m_iface && m_iface->captureSource() != d->source.index() )
	{
		int index = m_iface->captureSource();
		const Ifaces::Backend* backend = Factory::self()->backend();
		d->source = CaptureSource( index,
				backend->captureSourceNameForIndex( index ),
				backend->captureSourceDescriptionForIndex( index ) );
	}
	return d->source;
}

void SoundcardCapture::setCaptureSource( const CaptureSource& source )
{
	if( m_iface )
		m_iface->setCaptureSource( source.index() );
	else
		d->source = source;
}

void SoundcardCapture::setCaptureSource( int sourceIndex )
{
	if( m_iface )
		m_iface->setCaptureSource( sourceIndex );
	else
	{
		const Ifaces::Backend* backend = Factory::self()->backend();
		if( backend )
			d->source = CaptureSource( sourceIndex,
					backend->captureSourceNameForIndex( sourceIndex ),
					backend->captureSourceDescriptionForIndex( sourceIndex ) );
	}
}

bool SoundcardCapture::aboutToDeleteIface()
{
	int index = m_iface->captureSource();
	const Ifaces::Backend* backend = Factory::self()->backend();
	d->source = CaptureSource( index,
			backend->captureSourceNameForIndex( index ),
			backend->captureSourceDescriptionForIndex( index ) );
	return AbstractMediaProducer::aboutToDeleteIface();
}

void SoundcardCapture::setupIface( Ifaces::SoundcardCapture* iface )
{
	AbstractMediaProducer::setupIface( iface );

	m_iface = iface;
	if( !m_iface )
		return;

	// set up attributes
	m_iface->setCaptureSource( d->source.index() );
}

} //namespace Phonon

#include "soundcardcapture.moc"

// vim: sw=4 ts=4 tw=80 noet
