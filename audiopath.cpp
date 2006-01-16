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
#include "ifaces/audiopath.h"
#include "factory.h"

#include "audioeffect.h"
#include "abstractaudiooutput.h"

namespace Kdem2m
{
class AudioPath::Private
{
	public:
		Private()
			: channel( -1 )
		{ }

		int channel;
};

KDEM2M_OBJECT_IMPL( AudioPath )

int AudioPath::availableChannels() const
{
	return m_iface ? m_iface->availableChannels() : -1;
}

QString AudioPath::channelName( int channel ) const
{
	return m_iface ? m_iface->channelName( channel ) : QString();
}

bool AudioPath::selectChannel( int channel )
{
	if( m_iface )
		return m_iface->selectChannel( channel );
	d->channel = channel;
	return false;
}

int AudioPath::selectedChannel() const
{
	return m_iface ? m_iface->selectedChannel() : d->channel;
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

bool AudioPath::aboutToDeleteIface()
{
	d->channel = m_iface->selectedChannel();
	return true;
}

void AudioPath::setupIface()
{
	if( !m_iface )
		return;

	// set up attributes
	m_iface->selectChannel( d->channel );
}

} //namespace Kdem2m

#include "audiopath.moc"

// vim: sw=4 ts=4 tw=80 noet
