/*  This file is part of the KDE project
    Copyright (C) 2004-2006 Matthias Kretz <kretz@kde.org>

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

#ifndef Kdem2m_FAKE_BACKEND_H
#define Kdem2m_FAKE_BACKEND_H

#include "../../ifaces/backend.h"

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
}
namespace Fake
{

	class KDEM2M_EXPORT Backend : public Ifaces::Backend
	{
		Q_OBJECT
		public:
			Backend( QObject* parent );
			virtual ~Backend();

			virtual Ifaces::MediaObject*      createMediaObject( QObject* parent );
			virtual Ifaces::SoundcardCapture* createSoundcardCapture( QObject* parent );
			virtual Ifaces::ByteStream*       createByteStream( QObject* parent );

			virtual Ifaces::AudioPath*        createAudioPath( QObject* parent );
			virtual Ifaces::AudioEffect*      createAudioEffect( QObject* parent );
			virtual Ifaces::AudioOutput*      createAudioOutput( QObject* parent );
			virtual Ifaces::AudioDataOutput*  createAudioDataOutput( QObject* parent );
			virtual Ifaces::AudioFftOutput*   createAudioFftOutput( QObject* parent );

			virtual Ifaces::VideoPath*        createVideoPath( QObject* parent );
			virtual Ifaces::VideoEffect*      createVideoEffect( QObject* parent );

			virtual bool supportsVideo() const;
			virtual bool supportsOSD() const;
			virtual bool supportsSubtitles() const;
			virtual const KMimeType::List& knownMimeTypes() const;

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
			virtual int captureSourceCount() const;

			/**
			 * Returns the name of the capture source.
			 *
			 * \param index \f$0<\mathrm{index}\leq\mathrm{captureSourceCount}\f$
			 *
			 * \return The name of this capture source.
			 *
			 * \test The return value has to be non-empty.
			 */
			virtual QString captureSourceNameForIndex( int index ) const;

			/**
			 * Returns a description for the capture source.
			 *
			 * \param index \f$0<index\leq\f$ captureSourceCount
			 *
			 * \return The description for this capture source.
			 */
			virtual QString captureSourceDescriptionForIndex( int index ) const;

			// effects
			virtual const QStringList& availableAudioEffects() const;
			virtual const QStringList& availableVideoEffects() const;

			virtual const char* uiLibrary() const;
			//virtual const char* uiSymbol() const;

		private:
			KMimeType::List m_supportedMimeTypes;
			QStringList m_audioEffects, m_videoEffects;
	};
}} // namespace Kdem2m::Ifaces

// vim: sw=4 ts=4 noet tw=80
#endif // Kdem2m_FAKE_BACKEND_H
