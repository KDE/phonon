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
#ifndef Phonon_AUDIODATAOUTPUT_H
#define Phonon_AUDIODATAOUTPUT_H

#include <kdelibs_export.h>
#include "abstractaudiooutput.h"
#include "phonondefs.h"

template<class T> class QVector;

namespace Phonon
{
	class AudioDataOutputPrivate;
	namespace Ifaces
	{
		class AudioDataOutput;
	}

	/**
	 * @author Matthias Kretz <kretz@kde.org>
	 */
	class PHONON_EXPORT AudioDataOutput : public AbstractAudioOutput
	{
		Q_OBJECT
		Q_DECLARE_PRIVATE( AudioDataOutput )
		PHONON_HEIR( AudioDataOutput )
		public:
			/**
			 * @fn AudioDataOutput( QObject* parent = 0 );
			 */

			// Operations:
			void readBuffer( QVector<float>& buffer );
			void readBuffer( QVector<int>& buffer );

			// Attributes Getters:
			int availableSamples() const;

		public Q_SLOTS:
			// Attributes Setters:
	};
} //namespace Phonon

// vim: sw=4 ts=4 tw=80 noet
#endif // Phonon_AUDIODATAOUTPUT_H
