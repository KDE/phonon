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

#include "audiofftoutput.h"

namespace Kdem2m
{
namespace Fake
{
AudioFftOutput::AudioFftOutput( QObject* parent )
	: AbstractAudioOutput( parent )
{
}

AudioFftOutput::~AudioFftOutput()
{
}

//signals: void fourierTransformedData( const QVector<float>& spectrum );
const QVector<float>& AudioFftOutput::fourierTransformedData() const
{
	return m_data;
}

int AudioFftOutput::bandwidth() const
{
	return 0;
}

int AudioFftOutput::rate() const
{
	return 0;
}

int AudioFftOutput::setBandwidth( int newBandwidth )
{
	return 0;
}

int AudioFftOutput::setRate( int newRate )
{
	return 0;
}

}} //namespace Kdem2m::Fake

#include "audiofftoutput.moc"
// vim: sw=4 ts=4 noet
