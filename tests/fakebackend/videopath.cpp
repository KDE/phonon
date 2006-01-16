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

#include "videopath.h"
#include "videoeffect.h"
#include "abstractvideooutput.h"

namespace Phonon
{
namespace Fake
{

VideoPath::VideoPath( QObject* parent )
	: QObject( parent )
{
}

VideoPath::~VideoPath()
{
}

bool VideoPath::addOutput( Ifaces::AbstractVideoOutput* videoOutput )
{
	Q_ASSERT( videoOutput );
	AbstractVideoOutput* ao = qobject_cast<AbstractVideoOutput*>( videoOutput->qobject() );
	Q_ASSERT( ao );
	if( m_outputs.contains( ao ) )
		return false;
	m_outputs.append( ao );
	return true;
}

bool VideoPath::removeOutput( Ifaces::AbstractVideoOutput* videoOutput )
{
	Q_ASSERT( videoOutput );
	AbstractVideoOutput* ao = qobject_cast<AbstractVideoOutput*>( videoOutput->qobject() );
	Q_ASSERT( ao );
	if( m_outputs.removeAll( ao ) > 0 )
		return true;
	return false;
}

bool VideoPath::insertEffect( Ifaces::VideoEffect* newEffect, Ifaces::VideoEffect* insertBefore )
{
	Q_ASSERT( newEffect );
	VideoEffect* ae = qobject_cast<VideoEffect*>( newEffect->qobject() );
	Q_ASSERT( ae );
	VideoEffect* before = 0;
	if( insertBefore )
	{
		before = qobject_cast<VideoEffect*>( insertBefore->qobject() );
		Q_ASSERT( before );
		if( !m_effects.contains( before ) )
			return false;
		m_effects.insert( m_effects.indexOf( before ), ae );
	}
	else
		m_effects.append( ae );

	return true;
}

bool VideoPath::removeEffect( Ifaces::VideoEffect* effect )
{
	Q_ASSERT( effect );
	VideoEffect* ae = qobject_cast<VideoEffect*>( effect->qobject() );
	Q_ASSERT( ae );
	if( m_effects.removeAll( ae ) > 0 )
		return true;
	return false;
}
}}

#include "videopath.moc"
// vim: sw=4 ts=4 noet
