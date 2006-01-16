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
#ifndef Phonon_IFACES_AUDIOFFTOUTPUT_H
#define Phonon_IFACES_AUDIOFFTOUTPUT_H

#include "abstractaudiooutput.h"

template<class T> class QVector;

namespace Phonon
{
namespace Ifaces
{
	class AudioFftOutput : virtual public AbstractAudioOutput
	{
		public:
			virtual ~AudioFftOutput() {}

			// Operations:
			virtual const QVector<float>& fourierTransformedData() const = 0;

			// Attributes Getters:
			virtual int bandwidth() const = 0;
			virtual int rate() const = 0;

			// Attributes Setters:
			virtual int setBandwidth( int newBandwidth ) = 0;
			virtual int setRate( int newRate ) = 0;

		//signals:
			virtual void fourierTransformedData( const QVector<float>& spectrum ) = 0;

		private:
			class Private;
			Private* d;
	};
}} //namespace Phonon::Ifaces

// vim: sw=4 ts=4 tw=80 noet
#endif // Phonon_IFACES_AUDIOFFTOUTPUT_H
