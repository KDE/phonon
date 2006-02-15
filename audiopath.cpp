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
#include "audiopath.h"
#include "audiopath_p.h"
#include "ifaces/audiopath.h"
#include "factory.h"

#include "audioeffect.h"
#include "abstractaudiooutput.h"

namespace Phonon
{
PHONON_OBJECT_IMPL( AudioPath )

int AudioPath::availableChannels() const
{
	Q_D( const AudioPath );
	return d->iface() ? d->iface()->availableChannels() : -1;
}

QString AudioPath::channelName( int channel ) const
{
	Q_D( const AudioPath );
	return d->iface() ? d->iface()->channelName( channel ) : QString();
}

bool AudioPath::selectChannel( int channel )
{
	Q_D( AudioPath );
	if( d->iface() )
		return d->iface()->selectChannel( channel );
	d->channel = channel;
	return false;
}

int AudioPath::selectedChannel() const
{
	Q_D( const AudioPath );
	return d->iface() ? d->iface()->selectedChannel() : d->channel;
}

bool AudioPath::addOutput( AbstractAudioOutput* audioOutput )
{
	return iface()->addOutput( audioOutput->iface() );
}

bool AudioPath::removeOutput( AbstractAudioOutput* audioOutput )
{
	return iface()->removeOutput( audioOutput->iface() );
}

bool AudioPath::insertEffect( AudioEffect* newEffect, AudioEffect* insertBefore )
{
	return iface()->insertEffect( newEffect->iface(), insertBefore->iface() );
}

bool AudioPath::removeEffect( AudioEffect* effect )
{
	return iface()->removeEffect( effect->iface() );
}

bool AudioPathPrivate::aboutToDeleteIface()
{
	channel = iface()->selectedChannel();
	return true;
}

void AudioPath::setupIface()
{
	Q_D( AudioPath );
	Q_ASSERT( d->iface() );

	// set up attributes
	d->iface()->selectChannel( d->channel );
}

} //namespace Phonon

#include "audiopath.moc"

// vim: sw=4 ts=4 tw=80 noet
