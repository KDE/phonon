/*  This file is part of the KDE project
    Copyright (C) 2006 Matthias Kretz <kretz@kde.org>

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

#include "fadereffect.h"
#include "fadereffect_p.h"
#include "factory.h"

namespace Phonon
{
PHONON_HEIR_IMPL( FaderEffect, AudioEffect )

float FaderEffect::volume() const
{
	Q_D( const FaderEffect );
	return d->iface() ? d->iface()->volume() : d->currentVolume;
}

void FaderEffect::setVolume( float volume )
{
	Q_D( FaderEffect );
	if( d->iface() )
		d->iface()->setVolume( volume );
	else
		d->currentVolume = volume;
}

void FaderEffect::fadeIn( int fadeTime )
{
	fadeTo( 1.0, fadeTime );
}

void FaderEffect::fadeOut( int fadeTime )
{
	fadeTo( 0.0, fadeTime );
}

void FaderEffect::fadeTo( float volume, int fadeTime )
{
	Q_D( FaderEffect );
	if( iface() )
		d->iface()->fadeTo( volume, fadeTime );
	else
		d->currentVolume = volume;
}

bool FaderEffectPrivate::aboutToDeleteIface()
{
	if( iface() )
		currentVolume = iface()->volume();
	return true;
}

void FaderEffect::setupIface()
{
	Q_D( FaderEffect );
	Q_ASSERT( d->iface() );

	// set up attributes
	d->iface()->setVolume( d->currentVolume );
}
}

#include "fadereffect.moc"
// vim: sw=4 ts=4 noet
