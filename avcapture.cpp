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
#include "avcapture.h"
#include "ifaces/avcapture.h"
#include "factory.h"
#include "audiosource.h"
#include "videosource.h"
#include "ifaces/backend.h"

namespace Phonon
{
class AvCapture::Private
{
	public:
		Private()
		{ }

		AudioSource audioSource;
		VideoSource videoSource;
};

PHONON_HEIR_IMPL( AvCapture, AbstractMediaProducer )

const AudioSource& AvCapture::audioSource() const
{
	if( m_iface && m_iface->audioSource() != d->audioSource.index() )
	{
		int index = m_iface->audioSource();
		const Ifaces::Backend* backend = Factory::self()->backend();
		d->audioSource = AudioSource( index,
				backend->audioSourceName( index ),
				backend->audioSourceDescription( index ) );
	}
	return d->audioSource;
}

void AvCapture::setAudioSource( const AudioSource& audioSource )
{
	if( m_iface )
		m_iface->setAudioSource( audioSource.index() );
	else
		d->audioSource = audioSource;
}

void AvCapture::setAudioSource( int audioSourceIndex )
{
	if( m_iface )
		m_iface->setAudioSource( audioSourceIndex );
	else
	{
		const Ifaces::Backend* backend = Factory::self()->backend();
		if( backend )
			d->audioSource = AudioSource( audioSourceIndex,
					backend->audioSourceName( audioSourceIndex ),
					backend->audioSourceDescription( audioSourceIndex ) );
	}
}

const VideoSource& AvCapture::videoSource() const
{
	if( m_iface && m_iface->videoSource() != d->videoSource.index() )
	{
		int index = m_iface->videoSource();
		const Ifaces::Backend* backend = Factory::self()->backend();
		d->videoSource = VideoSource( index,
				backend->videoSourceName( index ),
				backend->videoSourceDescription( index ) );
	}
	return d->videoSource;
}

void AvCapture::setVideoSource( const VideoSource& videoSource )
{
	if( m_iface )
		m_iface->setVideoSource( videoSource.index() );
	else
		d->videoSource = videoSource;
}

void AvCapture::setVideoSource( int videoSourceIndex )
{
	if( m_iface )
		m_iface->setVideoSource( videoSourceIndex );
	else
	{
		const Ifaces::Backend* backend = Factory::self()->backend();
		if( backend )
			d->videoSource = VideoSource( videoSourceIndex,
					backend->videoSourceName( videoSourceIndex ),
					backend->videoSourceDescription( videoSourceIndex ) );
	}
}

bool AvCapture::aboutToDeleteIface()
{
	int index = m_iface->audioSource();
	const Ifaces::Backend* backend = Factory::self()->backend();
	d->audioSource = AudioSource( index,
			backend->audioSourceName( index ),
			backend->audioSourceDescription( index ) );
	return AbstractMediaProducer::aboutToDeleteIface();
}

void AvCapture::setupIface( Ifaces::AvCapture* iface )
{
	AbstractMediaProducer::setupIface( iface );

	m_iface = iface;
	if( !m_iface )
		return;

	// set up attributes
	m_iface->setAudioSource( d->audioSource.index() );
}

} //namespace Phonon

#include "avcapture.moc"

// vim: sw=4 ts=4 tw=80 noet
