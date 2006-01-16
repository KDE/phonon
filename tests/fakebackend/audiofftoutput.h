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
#ifndef Phonon_FAKE_AUDIOFFTOUTPUT_H
#define Phonon_FAKE_AUDIOFFTOUTPUT_H

#include "abstractaudiooutput.h"
#include "../../ifaces/audiofftoutput.h"
#include <QVector>

namespace Phonon
{
namespace Fake
{
	class AudioFftOutput : virtual public AbstractAudioOutput, virtual public Ifaces::AudioFftOutput
	{
		Q_OBJECT
		public:
			AudioFftOutput( QObject* parent );
			virtual ~AudioFftOutput();

			// Operations:
			virtual const QVector<float>& fourierTransformedData() const;

			// Attributes Getters:
			virtual int bandwidth() const;
			virtual int rate() const;

			// Attributes Setters:
			virtual int setBandwidth( int newBandwidth );
			virtual int setRate( int newRate );

		signals:
			void fourierTransformedData( const QVector<float>& spectrum );

		private:
			QVector<float> m_data;
	};
}} //namespace Phonon::Fake

// vim: sw=4 ts=4 tw=80 noet
#endif // Phonon_FAKE_AUDIOFFTOUTPUT_H
