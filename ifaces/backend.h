/*  This file is part of the KDE project
    Copyright (C) 2004-2005 Matthias Kretz <kretz@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License version 2 as published by the Free Software Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.

*/

#ifndef Kdem2m_IFACES_BACKEND_H
#define Kdem2m_IFACES_BACKEND_H

#include <QObject>
#include <kmimetype.h>
#include <kdelibs_export.h>

class KURL;

namespace Kdem2m
{
namespace Ifaces
{
	class MediaObject;
	class SoundcardCapture;
	class ByteStream;

	class AudioPath;
	class AudioEffect;
	class AudioOutput;
	class AudioDataOutput;
	class AudioFftOutput;

	class VideoPath;
	class VideoEffect;

	/**
	 * \brief Base class for all KDE Multimedia Backends
	 *
	 * This class provides the interface for Multimedia Backends. Use it to get
	 * a pointer to a new Player or VideoWidget.
	 *
	 * \author Matthias Kretz <kretz@kde.org>
	 * \since 4.0
	 */
	class KDEM2M_EXPORT Backend : public QObject
	{
		Q_OBJECT
		public:
			Backend( QObject* parent ) : QObject( parent ) {}
			virtual ~Backend() {}

			virtual MediaObject*      createMediaObject( QObject* parent ) = 0;
			virtual SoundcardCapture* createSoundcardCapture( QObject* parent ) = 0;
			virtual ByteStream*       createByteStream( QObject* parent ) = 0;

			virtual AudioPath*        createAudioPath( QObject* parent ) = 0;
			virtual AudioEffect*      createAudioEffect( QObject* parent ) = 0;
			virtual AudioOutput*      createAudioOutput( QObject* parent ) = 0;
			virtual AudioDataOutput*  createAudioDataOutput( QObject* parent ) = 0;
			virtual AudioFftOutput*   createAudioFftOutput( QObject* parent ) = 0;

			virtual VideoPath*        createVideoPath( QObject* parent ) = 0;
			virtual VideoEffect*      createVideoEffect( QObject* parent ) = 0;

			virtual bool supportsVideo() const = 0;
			virtual bool supportsOSD() const = 0;
			virtual bool supportsSubtitles() const = 0;
			virtual const KMimeType::List& knownMimeTypes() const = 0;

			/**
			 * Returns the number of available capture sources. An associated
			 * name and description can be found using captureSourceNameForIndex
			 * and captureSourceDescriptionForIndex.
			 *
			 * \return The number of available capture sources.
			 * \see captureSourceNameForIndex
			 * \see captureSourceDescriptionForIndex
			 *
			 * \test The return value has to be \f$\ge0\f$
			 */
			virtual int captureSourceCount() const = 0;

			/**
			 * Returns the name of the capture source.
			 *
			 * \param index \f$0<\mathrm{index}\leq\mathrm{captureSourceCount}\f$
			 *
			 * \return The name of this capture source.
			 *
			 * \test The return value has to be non-empty.
			 */
			virtual QString captureSourceNameForIndex( int index ) const = 0;

			/**
			 * Returns a description for the capture source.
			 *
			 * \param index \f$0<index\leq\f$ captureSourceCount
			 *
			 * \return The description for this capture source.
			 */
			virtual QString captureSourceDescriptionForIndex( int index ) const = 0;

			// effects
			virtual const QStringList& availableAudioEffects() const = 0;
			virtual const QStringList& availableVideoEffects() const = 0;

			virtual const char* uiLibrary() const = 0;
			virtual const char* uiSymbol() const { return 0; }

		private:
			class Private;
			Private * d;
	};
}} // namespace Kdem2m::Ifaces

// vim: sw=4 ts=4 noet tw=80
#endif // Kdem2m_IFACES_BACKEND_H
