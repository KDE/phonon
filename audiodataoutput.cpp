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
#include "audiodataoutput.h"
#include "audiodataoutput_p.h"
#include "ifaces/audiodataoutput.h"
#include "factory.h"

namespace Phonon
{

PHONON_HEIR_IMPL( AudioDataOutput, AbstractAudioOutput )

void AudioDataOutput::readBuffer( QVector<float>& buffer )
{
	iface()->readBuffer( buffer );
}

void AudioDataOutput::readBuffer( QVector<int>& buffer )
{
	iface()->readBuffer( buffer );
}

int AudioDataOutput::availableSamples() const
{
	Q_D( const AudioDataOutput );
	return d->iface() ? d->iface()->availableSamples() : d->availableSamples;
}

bool AudioDataOutputPrivate::aboutToDeleteIface()
{
	if( iface() )
	{
		availableSamples = iface()->availableSamples();
	}
	return AbstractAudioOutputPrivate::aboutToDeleteIface();
}

void AudioDataOutput::setupIface()
{
	Q_D( AudioDataOutput );
	Q_ASSERT( d->iface() );
	AbstractAudioOutput::setupIface();

	// set up attributes
}

} //namespace Phonon

#include "audiodataoutput.moc"

// vim: sw=4 ts=4 tw=80 noet
