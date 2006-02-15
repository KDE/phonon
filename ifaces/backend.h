/*  This file is part of the KDE project
    Copyright (C) 2004-2005-2006 Matthias Kretz <kretz@kde.org>

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

#ifndef Phonon_IFACES_BACKEND_H
#define Phonon_IFACES_BACKEND_H

#include <QObject>
#include <kmimetype.h>
#include <kdelibs_export.h>

class KUrl;

namespace Phonon
{
namespace Ifaces
{
	class MediaObject;
	class AvCapture;
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
	class PHONON_EXPORT Backend : public QObject
	{
		Q_OBJECT
		public:
			Backend( QObject* parent ) : QObject( parent ) {}
			virtual ~Backend() {}

			virtual MediaObject*      createMediaObject( QObject* parent ) = 0;
			virtual AvCapture*        createAvCapture( QObject* parent ) = 0;
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
			 * name and description can be found using audioSourceName
			 * and audioSourceDescription.
			 *
			 * \return The number of available capture sources.
			 * \see audioSourceName
			 * \see audioSourceDescription
			 *
			 * \test The return value has to be \f$\ge0\f$
			 */
			virtual int audioSourceCount() const = 0;

			/**
			 * Returns the name of the capture source.
			 *
			 * \param index \f$0<\mathrm{index}\leq\mathrm{audioSourceCount}\f$
			 *
			 * \return The name of this capture source.
			 *
			 * \test The return value has to be non-empty.
			 */
			virtual QString audioSourceName( int index ) const = 0;

			/**
			 * Returns a description for the capture source.
			 *
			 * \param index \f$0<index\leq\f$ audioSourceCount
			 *
			 * \return The description for this capture source.
			 */
			virtual QString audioSourceDescription( int index ) const = 0;

			virtual int audioSourceVideoIndex( int index ) const = 0;

			/**
			 * Returns the number of available capture sources. An associated
			 * name and description can be found using videoSourceName
			 * and videoSourceDescription.
			 *
			 * \return The number of available capture sources.
			 * \see videoSourceName
			 * \see videoSourceDescription
			 *
			 * \test The return value has to be \f$\ge0\f$
			 */
			virtual int videoSourceCount() const = 0;

			/**
			 * Returns the name of the capture source.
			 *
			 * \param index \f$0<\mathrm{index}\leq\mathrm{videoSourceCount}\f$
			 *
			 * \return The name of this capture source.
			 *
			 * \test The return value has to be non-empty.
			 */
			virtual QString videoSourceName( int index ) const = 0;

			/**
			 * Returns a description for the capture source.
			 *
			 * \param index \f$0<index\leq\f$ videoSourceCount
			 *
			 * \return The description for this capture source.
			 */
			virtual QString videoSourceDescription( int index ) const = 0;

			virtual int videoSourceAudioIndex( int index ) const = 0;

			// effects
			virtual const QStringList& availableAudioEffects() const = 0;
			virtual const QStringList& availableVideoEffects() const = 0;

			virtual const char* uiLibrary() const = 0;
			virtual const char* uiSymbol() const { return 0; }

		private:
			class Private;
			Private * d;
	};
}} // namespace Phonon::Ifaces

// vim: sw=4 ts=4 noet tw=80
#endif // Phonon_IFACES_BACKEND_H
