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

#ifndef PHONON_OBJECT_H
#define PHONON_OBJECT_H

#include <QObject>

#include <kdelibs_export.h>

namespace Phonon
{
	namespace Ifaces
	{
		class Object;
	}
	/**
	 * \internal
	 *
	 * \author Matthias Kretz <kretz@kde.org>
	 * \since 4.0
	 */
	class PHONON_EXPORT Object : public QObject
	{
		Q_OBJECT
		protected:
			/**
			 * Standard QObject constructor.
			 *
			 * @param parent QObject parent
			 */
			Object( QObject* parent = 0 );

		protected slots:
			/**
			 * \internal
			 * This method cleanly deletes the Iface object. It is called on
			 * destruction and before a backend change.
			 */
			virtual void deleteIface() = 0;

			/**
			 * \internal
			 * Creates the Iface object belonging to this class. For most cases the
			 * implementation is
			 * \code
			 * m_iface = Factory::self()->createClassName( this );
			 * return m_iface;
			 * \endcode
			 *
			 * This function should not be called except from slotCreateIface.
			 *
			 * \see slotCreateIface
			 */
			virtual void createIface() = 0;

		//private:
			//class Private;
			//Private* d;
	};
} //namespace Phonon

// vim: sw=4 ts=4 tw=80 noet
#endif // PHONON_OBJECT_H
