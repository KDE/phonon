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

#ifndef PHONON_BASE_H
#define PHONON_BASE_H

#include <QObject>

#include <kdelibs_export.h>

namespace Phonon
{
	class BasePrivate;
	/**
	 * \internal
	 *
	 * \author Matthias Kretz <kretz@kde.org>
	 * \since 4.0
	 */
	class PHONON_EXPORT Base : public QObject
	{
		Q_OBJECT
		Q_DECLARE_PRIVATE( Base )
		protected:
			/**
			 * Standard QObject constructor.
			 *
			 * \param d private object
			 * \param parent QObject parent
			 */
			Base( BasePrivate& d, QObject* parent = 0 );

			virtual ~Base();

		protected:
			BasePrivate* d_ptr;

		private:
			Q_PRIVATE_SLOT( d_func(), void deleteIface() );
			Q_PRIVATE_SLOT( d_func(), void createIface() );
	};
} //namespace Phonon

// vim: sw=4 ts=4 tw=80 noet
#endif // PHONON_BASE_H
