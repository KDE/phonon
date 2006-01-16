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

#ifndef PHONON_CAPTURESOURCE_H
#define PHONON_CAPTURESOURCE_H

class QString;

namespace Phonon
{
/**
 * @short Description for possible SoundcardCapture choices.
 *
 * There often are multiple choices of capture sources from the soundcard (or
 * sound system, e.g. soundserver). To let the enduser make a choice this class
 * provides the needed information.
 *
 * @author Matthias Kretz <kretz@kde.org>
 * @since 4.0
 * @see SoundcardCapture
 */
class CaptureSource
{
	friend class SoundcardCapture;
	friend class BackendCapabilities;
	public:
		CaptureSource();
		CaptureSource( const CaptureSource& );
		~CaptureSource();
		const CaptureSource& operator=( const CaptureSource& );
		bool operator==( const CaptureSource& ) const;

		/**
		 * Returns the name of the capture source.
		 *
		 * @return A string that should be presented to the user to
		 * choose the capture source.
		 */
		const QString& name() const;

		/**
		 * Returns a description of the capture source. This text should
		 * make clear what sound source this is, which is sometimes hard
		 * to describe or understand from just the name.
		 *
		 * @return A string describing the capture source.
		 */
		const QString& description() const;

		/**
		 * A unique identifier for this capture source. Used internally
		 * to distinguish between the capture sources.
		 *
		 * @return An integer that uniquely identifies every capture
		 * source.
		 */
		int index() const;

	protected:
		/**
		 * @internal
		 * Sets the data.
		 */
		CaptureSource( int index, const QString& name, const QString& description );

	private:
		class Private;
		Private* d;
};
} //namespace Phonon

#endif // PHONON_CAPTURESOURCE_H
