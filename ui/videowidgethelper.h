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
#ifndef Phonon_Ui_VIDEOWIDGETHELPER_H
#define Phonon_Ui_VIDEOWIDGETHELPER_H

#include "../abstractvideooutput.h"
#include <kdelibs_export.h>

class QString;

namespace Phonon
{
namespace Ifaces
{
	class AbstractVideoOutput;
}
namespace Ui
{
namespace Ifaces
{
	class VideoWidget;
}

class VideoWidget;

class PHONON_EXPORT VideoWidgetHelper : public Phonon::AbstractVideoOutput
{
	Q_OBJECT
	//cannot use macro: need special iface creation
	public:
		VideoWidgetHelper( VideoWidget* parent );
		~VideoWidgetHelper();
	protected:
		VideoWidgetHelper( Ifaces::VideoWidget* iface, VideoWidget* parent );
		virtual void ifaceDeleted();
		virtual Phonon::Ifaces::AbstractVideoOutput* createIface();
		virtual bool aboutToDeleteIface();
		virtual void setupIface();
	private:
		Ifaces::VideoWidget* iface();
		Ifaces::VideoWidget* m_iface;
		class Private;
		Private* d;
};

}} //namespace Phonon::Ui

// vim: sw=4 ts=4 tw=80 noet
#endif // Phonon_Ui_VIDEOWIDGETHELPER_H
