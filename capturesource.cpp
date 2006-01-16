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

#include "capturesource.h"
#include <QString>

namespace Kdem2m
{
class CaptureSource::Private
{
	public:
		Private( int a, const QString& b, const QString& c )
			: index( a )
			, name( b )
			, description( c )
		{
		}

		int index;
		QString name, description;
};

CaptureSource::CaptureSource()
	: d( new Private( -1, QString(), QString() ) )
{
}

CaptureSource::CaptureSource( const CaptureSource& rhs )
	: d( new Private( rhs.d->index, rhs.d->name, rhs.d->description ) )
{
}

CaptureSource::~CaptureSource()
{
	delete d;
	d = 0;
}

const CaptureSource& CaptureSource::operator=( const CaptureSource& rhs )
{
	delete d;
	d = new Private( rhs.d->index, rhs.d->name, rhs.d->description );
	return *this;
}

bool CaptureSource::operator==( const CaptureSource& rhs ) const
{
	return d->index == rhs.d->index && d->name == rhs.d->name && d->description == rhs.d->description;
}

CaptureSource::CaptureSource( int index, const QString& name, const QString& description )
	: d( new Private( index, name, description ) )
{
}

int CaptureSource::index() const
{
	return d->index;
}

const QString& CaptureSource::name() const
{
	return d->name;
}

const QString& CaptureSource::description() const
{
	return d->description;
}

} //namespace Kdem2m
// vim: sw=4 ts=4 noet
