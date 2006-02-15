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

#include "base.h"
#include "base_p.h"
#include "ifaces/base.h"
#include "factory.h"

namespace Phonon
{
	Base::Base( BasePrivate& d, QObject* parent )
		: QObject( parent )
		, d_ptr( &d )
	{
		d_ptr->q_ptr = this;
		d_ptr->setIface( 0 );
		connect( Factory::self(), SIGNAL( deleteYourObjects() ), SLOT( deleteIface() ) );
		connect( Factory::self(), SIGNAL( recreateObjects() ), SLOT( createIface() ) );
	}

	Base::~Base()
	{
		delete d_ptr;
		d_ptr = 0;
	}
} //namespace Phonon
#include "base.moc"
// vim: sw=4 ts=4 noet
