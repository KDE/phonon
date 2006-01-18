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
#include "abstractvideooutput.h"
#include "ifaces/abstractvideooutput.h"
#include "factory.h"

namespace Phonon
{
class AbstractVideoOutput::Private
{
	public:
		Private()
		{ }

		QString name;
};

PHONON_ABSTRACTBASE_IMPL( AbstractVideoOutput )

bool AbstractVideoOutput::aboutToDeleteIface()
{
	//if( m_iface )
	//{
	//}
	return true;
}

void AbstractVideoOutput::setupIface( Ifaces::AbstractVideoOutput* iface )
{
	m_iface = iface;
	//if( !m_iface )
		//return;
}

} //namespace Phonon

#include "abstractvideooutput.moc"

// vim: sw=4 ts=4 tw=80 noet
