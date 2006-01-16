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
#include "audiooutput.h"
#include "ifaces/audiooutput.h"
#include "factory.h"

#include <kglobal.h>
#include <kinstance.h>
#include <kaboutdata.h>

namespace Kdem2m
{
class AudioOutput::Private
{
	public:
		Private()
			: volume( 1.0 )
		{ 
			const KAboutData* ad = KGlobal::instance()->aboutData();
			if( ad )
				name = ad->programName();
			else
				name = KGlobal::instance()->instanceName();
		}

		QString name;
		float volume;
};

KDEM2M_HEIR_IMPL( AudioOutput, AbstractAudioOutput, AbstractAudioOutput )

QString AudioOutput::name() const
{
	return m_iface ? m_iface->name() : d->name;
}

void AudioOutput::setName( const QString& newName )
{
	if( m_iface )
		d->name = m_iface->setName( newName );
	else
		d->name = newName;
}

float AudioOutput::volume() const
{
	return m_iface ? m_iface->volume() : d->volume;
}

void AudioOutput::setVolume( float newVolume )
{
	if( m_iface )
		d->volume = m_iface->setVolume( newVolume );
	else
		d->volume = newVolume;
}

bool AudioOutput::aboutToDeleteIface()
{
	if( m_iface )
	{
		d->name = m_iface->name();
		d->volume = m_iface->volume();
	}
	return AbstractAudioOutput::aboutToDeleteIface();
}

void AudioOutput::setupIface()
{
	AbstractAudioOutput::setupIface();

	if( !m_iface )
		return;

	connect( m_iface->qobject(), SIGNAL( volumeChanged( float ) ), SIGNAL( volumeChanged( float ) ) );

	// set up attributes
	m_iface->setName( d->name );
	m_iface->setVolume( d->volume );
}

} //namespace Kdem2m

#include "audiooutput.moc"

// vim: sw=4 ts=4 tw=80 noet
