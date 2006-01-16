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
#ifndef Kdem2m_SOUNDCARDCAPTURE_H
#define Kdem2m_SOUNDCARDCAPTURE_H

#include "abstractmediaproducer.h"
#include <kdelibs_export.h>
#include "kdem2mdefs.h"

class QString;
class QStringList;

namespace Kdem2m
{
	class CaptureSource;
	namespace Ifaces
	{
		class SoundcardCapture;
	}

	/**
	 * @short Media data from a soundcard or soundserver.
	 *
	 * This class gives you access to the capture capabilities of the
	 * soundsystem/soundcard. There might be more than only one possible capture
	 * source. The list of available sources is available through
	 * BackendCapabilities::availableSoundcardCaptureSources.
	 *
	 * @author Matthias Kretz <kretz@kde.org>
	 * @since 4.0
	 * @see BackendCapabilities::availableSoundcardCaptureSources
	 */
	class KDEM2M_EXPORT SoundcardCapture : public AbstractMediaProducer
	{
		Q_OBJECT
		KDEM2M_HEIR( SoundcardCapture, AbstractMediaProducer )
		public:
			/**
			 * Returns the currently used capture source.
			 *
			 * @see CaptureSource
			 * @see setCaptureSource( const CaptureSource& )
			 * @see setCaptureSource( int )
			 */
			const CaptureSource& captureSource() const;

		public slots:
			/**
			 * Sets the capture source to use.
			 *
			 * @param source An object of class CaptureSource. A list of
			 * available objects can be queried from
			 * BackendCapabilities::availableSoundcardCaptureSources.
			 *
			 * @see captureSource
			 * @see setCaptureSource( int )
			 */
			void setCaptureSource( const CaptureSource& source );

			/**
			 * Sets the capture source to use.
			 *
			 * @param sourceIndex An index corresponding an object of class
			 * CaptureSource. A list of available objects can be queried from
			 * BackendCapabilities::availableSoundcardCaptureSources.
			 *
			 * @see captureSource
			 * @see setCaptureSource( const CaptureSource& )
			 */
			void setCaptureSource( int sourceIndex );
	};
} //namespace Kdem2m

// vim: sw=4 ts=4 tw=80 noet
#endif // Kdem2m_SOUNDCARDCAPTURE_H
