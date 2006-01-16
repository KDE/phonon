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

#include "videowidget.h"
#include "factory.h"
#include "videowidgethelper.h"

#include <phonon/ifaces/ui/videowidget.h>

namespace Phonon
{
namespace Ui
{
class VideoWidget::Private
{
	public:
		Private()
			: helper( 0 )
			, fullscreen( false )
		{ }

		VideoWidgetHelper* helper;
		bool fullscreen;
};

VideoWidget::VideoWidget( QWidget* parent )
	: QWidget( parent )
	, m_iface( 0 )
	, d( new Private() )
{
	slotCreateIface();
	connect( Factory::self(), SIGNAL( deleteYourObjects() ), SLOT( slotDeleteIface() ) );
	connect( Factory::self(), SIGNAL( recreateObjects() ), SLOT( slotCreateIface() ) );
}

VideoWidget::VideoWidget( Ui::Ifaces::VideoWidget* iface, QWidget* parent )
	: QWidget( parent )
	, m_iface( iface )
	, d( new Private() )
{
	connect( Factory::self(), SIGNAL( deleteYourObjects() ), SLOT( slotDeleteIface() ) );
	connect( Factory::self(), SIGNAL( recreateObjects() ), SLOT( slotCreateIface() ) );
}

VideoWidget::~VideoWidget()
{
	slotDeleteIface();
	delete d;
	d = 0;
}

AbstractVideoOutput* VideoWidget::videoOutput()
{
	return d->helper;
}

bool VideoWidget::isFullscreen() const
{
	return m_iface ? m_iface->isFullscreen() : d->fullscreen;
}

void VideoWidget::setFullscreen( bool newFullscreen )
{
	if( m_iface )
		d->fullscreen = m_iface->setFullscreen( newFullscreen );
	else
		d->fullscreen = newFullscreen;
}

bool VideoWidget::aboutToDeleteIface()
{
	if( m_iface )
		d->fullscreen = m_iface->isFullscreen();
	return true;
}

void VideoWidget::slotDeleteIface()
{
	if( aboutToDeleteIface() )
	{
		delete m_iface;
		m_iface = 0;
	}
}

void VideoWidget::slotCreateIface()
{
	if( !m_iface )
		m_iface = createIface();
	setupIface();
}

Ui::Ifaces::VideoWidget* VideoWidget::createIface()
{
  	delete m_iface;
  	m_iface = Factory::self()->createVideoWidget( this );
	return m_iface;
}

void VideoWidget::setupIface()
{
	d->helper = new VideoWidgetHelper( this );
	m_iface->setFullscreen( d->fullscreen );
}

Ui::Ifaces::VideoWidget* VideoWidget::iface()
{
	if( !m_iface )
		slotCreateIface();
	return m_iface;
}

}} //namespace Phonon::Ui

#include "videowidget.moc"

// vim: sw=4 ts=4 tw=80 noet
