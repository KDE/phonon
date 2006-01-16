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
#ifndef Kdem2m_AUDIOPATH_H
#define Kdem2m_AUDIOPATH_H

#include <QObject>
#include <kdelibs_export.h>
#include "kdem2mdefs.h"

namespace Kdem2m
{
	class AudioEffect;
	class AbstractAudioOutput;

	namespace Ifaces
	{
		class AudioPath;
	}

	/**
	 * @short A class to route and modify audio data.
	 *
	 * This class is used to define where the audio data is sent to, what
	 * effects are applied and what audio channels is to be used.
	 *
	 * For simple usage an instance of this class is added to a AbstractMediaProducer
	 * and an AbstractAudioOutput subclass (such as AudioOutput) added with
	 * addOutput.
	 * @code
	 * AudioPath* path = new AudioPath( this );
	 * mediaProducer->addAudioPath( path );
	 * path->addOutput( audioOutput );
	 * @endcode
	 *
	 * It is best to add the path to a AbstractMediaProducer after construction and
	 * before calling any other methods.
	 *
	 * @author Matthias Kretz <kretz@kde.org>
	 * @since 4.0
	 * @see VideoPath
	 */
	class KDEM2M_EXPORT AudioPath : public QObject
	{
		friend class AbstractMediaProducer;
		Q_OBJECT
		Q_PROPERTY( int channel READ selectedChannel WRITE selectChannel )
		KDEM2M_OBJECT( AudioPath )

		public:
			/**
			 * Returns the number of available audio channels. Audio files
			 * normally only have one channel. Channels are interesting for
			 * media like DVDs where you can have different spoken languages in
			 * different audio channels.
			 *
			 * You have to insert the AudioPath object into a AbstractMediaProducer
			 * before this method can give a usefull answer. This is obvious if
			 * you recall that, of course, the audio information is in the media
			 * data (which is represented by the AbstractMediaProducer object).
			 *
			 * @return The number of channels. @c 0 means no audio channels at
			 * all. @c -1 means it is not known yet, your program should ask
			 * again at a later point.
			 *
			 * @see channelName
			 * @see selectChannel
			 */
			int availableChannels() const;

			/**
			 * Returns the name of the given channel number.
			 *
			 * @param channel A number greater than @c 0 but less than or equal to
			 * availableChannels (0 < @p channel <= availableChannels)
			 *
			 * @return The user visible name for the audio channel. This string
			 * should be used by the end user to select the channel.
			 *
			 * @see availableChannels
			 */
			QString channelName( int channel ) const;

			/**
			 * Selects the given channel.
			 *
			 * @param channel A number greater than @c 0 but less than or equal to
			 * availableChannels (0 < @p channel <= availableChannels)
			 *
			 * @return @c true if the call succeeded.
			 * @return @c false if the call failed. This should only happen if
			 * @p channel is out of range.
			 *
			 * @see availableChannels
			 * @see selectedChannel
			 */
			bool selectChannel( int channel );

			/**
			 * Returns the index of the currently selected channel.
			 *
			 * @return The index of the currently selected channel.
			 *
			 * @see selectChannel
			 */
			int selectedChannel() const;

			/**
			 * Adds an audio output at the "end" of the audio path. This sends
			 * all audio data from the selected channel, after they are
			 * processed in the effects, to the given audio output object.
			 *
			 * @param audioOutput An object of a subclass of AbstractAudioOutput.
			 * This can be standard soundcard outputs or special outputs that
			 * give your program access to the audio data (or frequency spectrum).
			 *
			 * @return Returns whether the call was successfull.
			 *
			 * @see removeOutput
			 * @see AudioOutput
			 * @see AudioFftOutput
			 * @see AudioDataOutput
			 */
			bool addOutput( AbstractAudioOutput* audioOutput );

			/**
			 * Removes the audio output, thereby disconnecting the audio data
			 * flow.
			 *
			 * If the audio output object gets deleted while it is still
			 * connected the output will be removed automatically.
			 *
			 * @param audioOutput your audio output object
			 *
			 * @return Returns whether the call was successfull. If it returns
			 * @c false the audio output could not be found in the path, meaning
			 * it was not connected at all.
			 *
			 * @see addOutput
			 */
			bool removeOutput( AbstractAudioOutput* audioOutput );

			/**
			 * Inserts an effect into the audio path.
			 *
			 * @param newEffect An object of a subclass of AudioEffect.
			 *
			 * @param insertBefore If you already inserted an effect you can
			 * tell with this parameter in which order the audio data gets
			 * processed. If this is @c 0 the effect is appended at the end of
			 * the processing list. If the effect has not been inserted before
			 * the method will do nothing and return @c false.
			 *
			 * @return Returns whether the effect could be inserted at the
			 * specified position. If @c false is returned the effect was not
			 * inserted.
			 *
			 * @see removeEffect
			 * @see AudioEffect
			 */
			bool insertEffect( AudioEffect* newEffect, AudioEffect* insertBefore = 0 );

			/**
			 * Removes an effect from the audio path.
			 *
			 * If the effect gets deleted while it is still connected the effect
			 * will be removed automatically.
			 *
			 * @param effect The effect to be removed.
			 *
			 * @return Returns whether the call was successfull. If it returns
			 * @c false the effect could not be found in the path, meaning it
			 * has not been inserted before.
			 *
			 * @see insertEffect
			 */
			bool removeEffect( AudioEffect* effect );
	};
} //namespace Kdem2m

// vim: sw=4 ts=4 tw=80 noet
#endif // Kdem2m_AUDIOPATH_H
