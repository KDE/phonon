/*  This file is part of the KDE project
    Copyright (C) 2005-2006 Matthias Kretz <kretz@kde.org>

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
#include "videowidgethelper.h"
#include "videowidget.h"
#include "../ifaces/ui/videowidget.h"
#include "factory.h"

namespace Phonon
{
namespace Ui
{
class VideoWidgetHelper::Private
{
	public:
		Private( VideoWidget* vw )
			: videoWidget( vw )
		{ }

		VideoWidget* videoWidget;
};

//cannot use macro: need special iface creation

VideoWidgetHelper::VideoWidgetHelper( VideoWidget* parent )
	: AbstractVideoOutput( false, parent )
	, d( new Private( parent ) )
{
	createIface();
}

VideoWidgetHelper::~VideoWidgetHelper()
{
	delete d;
	d = 0;
}

bool VideoWidgetHelper::aboutToDeleteIface()
{
	return AbstractVideoOutput::aboutToDeleteIface();
}

void VideoWidgetHelper::ifaceDeleted()
{
	m_iface = 0;
	AbstractVideoOutput::ifaceDeleted();
}

void VideoWidgetHelper::createIface()
{
	m_iface = d->videoWidget->iface();
	setupIface( m_iface );
}

void VideoWidgetHelper::setupIface( Ifaces::VideoWidget* iface )
{
	AbstractVideoOutput::setupIface( iface );

	m_iface = iface;
	//if( !m_iface )
		//return;
}

Ui::Ifaces::VideoWidget* VideoWidgetHelper::iface()
{
	if( !m_iface )
		createIface();
	return m_iface;
}

}} //namespace Phonon::Ui

#include "videowidgethelper.moc"

// vim: sw=4 ts=4 tw=80 noet
