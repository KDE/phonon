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
#ifndef Phonon_AUDIOOUTPUT_H
#define Phonon_AUDIOOUTPUT_H

#include "abstractaudiooutput.h"
#include "mixeriface.h"
#include <kdelibs_export.h>
#include "phonondefs.h"

class QString;

namespace Phonon
{
	namespace Ifaces
	{
		class AudioOutput;
	}

	class PHONON_EXPORT AudioOutput : public AbstractAudioOutput, public MixerIface
	{
		Q_OBJECT
		PHONON_HEIR( AudioOutput )
		public:
			// Attributes Getters:
			QString name() const;
			float volume() const;

		public Q_SLOTS:
			// Attributes Setters:
			void setName( const QString& newName );
			void setVolume( float newVolume );

		Q_SIGNALS:
			void volumeChanged( float newVolume );
	};
} //namespace Phonon

// vim: sw=4 ts=4 tw=80 noet
#endif // Phonon_AUDIOOUTPUT_H
