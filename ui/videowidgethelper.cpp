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
#include "videowidgethelper_p.h"
#include "videowidget.h"
#include "../ifaces/ui/videowidget.h"
#include "factory.h"

namespace Phonon
{
namespace Ui
{
//cannot use macro: need special iface creation

VideoWidgetHelper::VideoWidgetHelper( VideoWidget* parent )
	: AbstractVideoOutput( *new VideoWidgetHelperPrivate( parent ), parent )
{
}

bool VideoWidgetHelperPrivate::aboutToDeleteIface()
{
	return AbstractVideoOutputPrivate::aboutToDeleteIface();
}

void VideoWidgetHelperPrivate::createIface()
{
	Q_Q( VideoWidgetHelper );
	Q_ASSERT( iface_ptr == 0 );
	setIface( videoWidget->iface() );
	q->setupIface();
}

void VideoWidgetHelper::setupIface()
{
	Q_D( VideoWidgetHelper );
	Q_ASSERT( d->iface() );
	AbstractVideoOutput::setupIface();
}

Ui::Ifaces::VideoWidget* VideoWidgetHelper::iface()
{
	Q_D( VideoWidgetHelper );
	if( !d->iface() )
		d->createIface();
	return d->iface();
}

}} //namespace Phonon::Ui

#include "videowidgethelper.moc"

// vim: sw=4 ts=4 tw=80 noet
