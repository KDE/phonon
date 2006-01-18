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
#include "audiofftoutput.h"
#include "ifaces/audiofftoutput.h"
#include "factory.h"

#include <QVector>

namespace Phonon
{
class AudioFftOutput::Private
{
	public:
		Private()
		{ }

		int bandwidth;
		int rate;
};

PHONON_HEIR_IMPL( AudioFftOutput, AbstractAudioOutput )

QVector<float> AudioFftOutput::fourierTransformedData() const
{
	return m_iface ? m_iface->fourierTransformedData() : QVector<float>();
}

int AudioFftOutput::bandwidth() const
{
	return m_iface ? m_iface->bandwidth() : d->bandwidth;
}

int AudioFftOutput::setBandwidth( int newBandwidth )
{
	if( m_iface )
		d->bandwidth = m_iface->setBandwidth( newBandwidth );
	else
		d->bandwidth = newBandwidth;
	return d->bandwidth;
}

int AudioFftOutput::rate() const
{
	return m_iface ? m_iface->rate() : d->rate;
}

void AudioFftOutput::setRate( int newRate )
{
	if( m_iface )
		d->rate = m_iface->setRate( newRate );
	else
		d->rate = newRate;
}

bool AudioFftOutput::aboutToDeleteIface()
{
	if( m_iface )
	{
		d->bandwidth = m_iface->bandwidth();
		d->rate = m_iface->rate();
	}
	return AbstractAudioOutput::aboutToDeleteIface();
}

void AudioFftOutput::setupIface( Ifaces::AudioFftOutput* iface )
{
	AbstractAudioOutput::setupIface( iface );

	m_iface = iface;
	if( !m_iface )
		return;

	connect( m_iface->qobject(), SIGNAL( fourierTransformedData( const QVector<float>& ) ),
			SIGNAL( fourierTransformedData( const QVector<float>& ) ) );
	// set up attributes
	d->bandwidth = m_iface->setBandwidth( d->bandwidth );
	m_iface->setRate( d->rate );
}

} //namespace Phonon

#include "audiofftoutput.moc"

// vim: sw=4 ts=4 tw=80 noet
