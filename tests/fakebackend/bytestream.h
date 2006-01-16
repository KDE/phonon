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
#ifndef Kdem2m_FAKE_BYTESTREAM_H
#define Kdem2m_FAKE_BYTESTREAM_H

#include "abstractmediaproducer.h"
#include "../../ifaces/bytestream.h"

namespace Kdem2m
{
namespace Fake
{
	class ByteStream : virtual public AbstractMediaProducer, virtual public Ifaces::ByteStream
	{
		Q_OBJECT
		public:
			ByteStream( QObject* parent );
			virtual ~ByteStream();

			//virtual void writeBuffer( const QByteArray& buffer );

			/**
			 * Called when there will be no more calls to writeBuffer
			 */
			//virtual void endOfData();

			/**
			 * Sets the total number of bytes that will be streamed via
			 * writeBuffer
			 */
			//virtual void toBeWritten();

		//signals:
			//void bufferUnderrun();

		private:
			//QTimer* m_decBufferTimer;
	};
}} //namespace Kdem2m::Fake

// vim: sw=4 ts=4 tw=80 noet
#endif // Kdem2m_FAKE_BYTESTREAM_H
