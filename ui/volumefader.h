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

#ifndef KDEM2M_UI_VOLUMEFADER_H
#define KDEM2M_UI_VOLUMEFADER_H

#include <QWidget>
#include <kdelibs_export.h>

namespace Kdem2m
{
class AudioOutput;

namespace Ui
{

/**
 * \short Widget providing a fader to control the volume of an AudioOutput.
 *
 * \author Matthias Kretz <kretz@kde.org>
 * \since 4.0
 */
class KDEM2M_EXPORT VolumeFader : public QWidget
{
	Q_OBJECT
	public:
		VolumeFader( QWidget* parent = 0 );
		~VolumeFader();

	public slots:
		void setOrientation( Qt::Orientation );
		void setAudioOutput( AudioOutput* );

	private slots:
		void outputDestroyed();
		void sliderChanged( int );
		void volumeChanged( float );

	private:
		class Private;
		Private* d;
};

}} // namespace Kdem2m::Ui

// vim: sw=4 ts=4 tw=80
#endif // KDEM2M_UI_VOLUMEFADER_H
