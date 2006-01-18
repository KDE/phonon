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
#ifndef PHONON_BYTESTREAM_H
#define PHONON_BYTESTREAM_H

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
		PHONON_HEIR( ByteStream )
		public:
			long totalTime() const;
			long remainingTime() const;
			long aboutToFinishTime() const;
			long streamSize() const;

			/**
			 * Returns whether you need to react on the seekStream signal when
			 * it is emitted.
			 *
			 * \return \c true You have to seek in the datastream when
			 * seekStream is emitted.
			 * \return \c false You only have to write the stream to writeData
			 * in one sequence.
			 */
			bool streamSeekable() const;

		public slots:
			/**
			 * Tell the object whether you will support seeking in the
			 * datastream. If you do, you have to react to the seekStream
			 * signal. If you don't you can savely ignore that signal.
			 *
			 * \param seekable Whether you are able to seek in the datastream
			 * and provide the writeData method with the data at arbitrary
			 * positions.
			 *
			 * \see streamSeekable
			 * \see seekStream
			 */
			void setStreamSeekable( bool seekable );

			void writeData( const QByteArray& data );

			/**
			 * Sets how many bytes the stream has. The Size is counted from the
			 * start of the stream until the end and not from the point in time
			 * when the slot is called.
			 *
			 * \param streamSize The size of the stream in bytes. The special
			 * value \c -1 is used for "unknown size".
			 * 
			 * \see streamSize
			 * \see endOfData
			 */
			void setStreamSize( long streamSize );

			/**
			 * Call this slot after your last call to writeData. This is needed
			 * for the ByteStream to properly emit the finished and
			 * aboutToFinish signals. If the internal buffer is too small to
			 * cover the aboutToFinishTime the streamSize is used for the
			 * aboutToFinish signal. If the streamSize is unknown and the buffer
			 * is too small to emit the aboutToFinish signal in time it is
			 * emitted as soon as possible.
			 *
			 * \see setStreamSize
			 */
			void endOfData();
			void setAboutToFinishTime( long newAboutToFinishTime );

		signals:
			void finished();

			/**
			 * Emitted when the stream is about to finish. The aboutToFinishTime
			 * is not guaranteed to be correct as sometimes the stream time
			 * until the end is not known early enough. Never rely on the
			 * aboutToFinishTime you set, but use the \p msec parameter instead.
			 *
			 * \param msec The time in milliseconds until the stream finishes.
			 */
			void aboutToFinish( long msec );

			void length( long length );

			/**
			 * Emitted when the ByteStream object needs more data to process.
			 * Your slot should not take too long to call writeData because otherwise
			 * the stream might drop.
			 */
			void needData();

			/**
			 * Emitted when the ByteStream object has enough data to process.
			 * This means you do not need to call writeData anymore until
			 * needData is emitted.
			 */
			void enoughData();

			/**
			 * Emitted when the ByteStream needs you to continue streaming data
			 * at a different position in the stream. This happens when seek(
			 * long ) is called and the needed data is not in the internal
			 * buffer.
			 *
			 * \param age The number of bytes since the start of the stream.
			 *            Your next call to writeData is expected to contain the
			 *            data from this position on.
			 *
			 * \see setStreamSeekable
			 */
			void seekStream( long age );
	};
} //namespace Phonon

// vim: sw=4 ts=4 tw=80 noet
#endif // PHONON_BYTESTREAM_H
