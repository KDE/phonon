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
#ifndef Phonon_BYTESTREAM_H
#define Phonon_BYTESTREAM_H

#include "abstractmediaproducer.h"
#include <kdelibs_export.h>
#include "phonondefs.h"

class QString;
class QStringList;

namespace Phonon
{
	namespace Ifaces
	{
		class ByteStream;
	}

	class PHONON_EXPORT ByteStream : public AbstractMediaProducer
	{
		Q_OBJECT
		PHONON_HEIR( ByteStream, AbstractMediaProducer )
		public:
			long totalTime() const;
			long remainingTime() const;
			long aboutToFinishTime() const;

		public slots:
			void writeData( QVector<quint8> );
			void setAboutToFinishTime( long newAboutToFinishTime );

		signals:
			void finished();
			void aboutToFinish( long msec );
			void length( long length );
	};
} //namespace Phonon

// vim: sw=4 ts=4 tw=80 noet
#endif // Phonon_BYTESTREAM_H
