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
	Q_D( AudioPath );
	if( iface()->addOutput( audioOutput->iface() ) )
	{
		d->outputs << audioOutput;
		return true;
	}
	return false;
}

bool AudioPath::removeOutput( AbstractAudioOutput* audioOutput )
{
	Q_D( AudioPath );
	if( iface()->removeOutput( audioOutput->iface() ) )
	{
		d->outputs.removeAll( audioOutput );
		return true;
	}
	return false;
}

const QList<AbstractAudioOutput*>& AudioPath::outputs() const
{
	Q_D( const AudioPath );
	return d->outputs;
}

bool AudioPath::insertEffect( AudioEffect* newEffect, AudioEffect* insertBefore )
{
	Q_D( AudioPath );
	if( iface()->insertEffect( newEffect->iface(), insertBefore->iface() ) )
	{
		if( insertBefore )
			d->effects.insert( d->effects.indexOf( insertBefore ), newEffect );
		else
			d->effects << newEffect;
		return true;
	}
	return false;
}

bool AudioPath::removeEffect( AudioEffect* effect )
{
	Q_D( AudioPath );
	if( iface()->removeEffect( effect->iface() ) )
	{
		d->effects.removeAll( effect );
		return true;
	}
	return false;
}

const QList<AudioEffect*>& AudioPath::effects() const
{
	Q_D( const AudioPath );
	return d->effects;
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

	QList<AbstractAudioOutput*> outputList = d->outputs;
	foreach( AbstractAudioOutput* output, outputList )
		if( !d->iface()->addOutput( output->iface() ) )
			d->outputs.removeAll( output );

	QList<AudioEffect*> effectList = d->effects;
	foreach( AudioEffect* effect, effectList )
		if( !d->iface()->insertEffect( effect->iface() ) )
			d->effects.removeAll( effect );
}

} //namespace Phonon

#include "audiopath.moc"

// vim: sw=4 ts=4 tw=80 noet
