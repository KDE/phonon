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
#include "videopath.h"
#include "videopath_p.h"
#include "ifaces/videopath.h"
#include "factory.h"

#include "videoeffect.h"
#include "abstractvideooutput.h"

namespace Phonon
{
PHONON_OBJECT_IMPL( VideoPath )

bool VideoPath::addOutput( AbstractVideoOutput* videoOutput )
{
	Q_D( VideoPath );
	if( d->outputs.contains( videoOutput ) )
		return false;

	if( iface() && d->iface()->addOutput( videoOutput->iface() ) )
	{
		connect( videoOutput, SIGNAL( destroyed( Base* ) ), SLOT( outputDestroyed( Base* ) ) );
		d->outputs << videoOutput;
		return true;
	}
	return false;
}

bool VideoPath::removeOutput( AbstractVideoOutput* videoOutput )
{
	Q_D( VideoPath );
	if( !d->outputs.contains( videoOutput ) )
		return false;

	if( iface()->removeOutput( videoOutput->iface() ) )
	{
		d->outputs.removeAll( videoOutput );
		return true;
	}
	return false;
}

const QList<AbstractVideoOutput*>& VideoPath::outputs() const
{
	Q_D( const VideoPath );
	return d->outputs;
}

bool VideoPath::insertEffect( VideoEffect* newEffect, VideoEffect* insertBefore )
{
	// effects may be added multiple times, but that means the objects are
	// different (the class is still the same)
	Q_D( VideoPath );
	if( d->effects.contains( newEffect ) )
		return false;

	if( iface() && d->iface()->insertEffect( newEffect->iface(), insertBefore->iface() ) )
	{
		connect( newEffect, SIGNAL( destroyed( Base* ) ), SLOT( effectDestroyed( Base* ) ) );
		if( insertBefore )
			d->effects.insert( d->effects.indexOf( insertBefore ), newEffect );
		else
			d->effects << newEffect;
		return true;
	}
	return false;
}

bool VideoPath::removeEffect( VideoEffect* effect )
{
	Q_D( VideoPath );
	if( !d->effects.contains( effect ) )
		return false;

	if( iface()->removeEffect( effect->iface() ) )
	{
		d->effects.removeAll( effect );
		return true;
	}
	return false;
}

const QList<VideoEffect*>& VideoPath::effects() const
{
	Q_D( const VideoPath );
	return d->effects;
}

bool VideoPathPrivate::aboutToDeleteIface()
{
	return true;
}

void VideoPath::setupIface()
{
	Q_D( VideoPath );
	Q_ASSERT( d->iface() );

	// set up attributes
	QList<AbstractVideoOutput*> outputList = d->outputs;
	foreach( AbstractVideoOutput* output, outputList )
		if( !d->iface()->addOutput( output->iface() ) )
			d->outputs.removeAll( output );

	QList<VideoEffect*> effectList = d->effects;
	foreach( VideoEffect* effect, effectList )
		if( !d->iface()->insertEffect( effect->iface() ) )
			d->effects.removeAll( effect );
}

void VideoPathPrivate::effectDestroyed( Base* o )
{
	// this method is called from Phonon::Base::~Base(), meaning the VideoEffect
	// dtor has already been called, also virtual functions don't work anymore
	// (therefore qobject_cast can only downcast from Base)
	Q_ASSERT( o );
	VideoEffect* videoEffect = static_cast<VideoEffect*>( o );
	if( iface() )
	{
		iface()->removeEffect( videoEffect->iface() );
		effects.removeAll( videoEffect );
	}
}

void VideoPathPrivate::outputDestroyed( Base* o )
{
	Q_ASSERT( o );
	AbstractVideoOutput* output = static_cast<AbstractVideoOutput*>( o );
	if( iface() )
	{
		iface()->removeOutput( output->iface() );
		outputs.removeAll( output );
	}
}

} //namespace Phonon

#include "videopath.moc"

// vim: sw=4 ts=4 tw=80 noet
