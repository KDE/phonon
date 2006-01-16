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
#ifndef Kdem2m_FAKE_ABSTRACTVIDEOOUTPUTBASE_H
#define Kdem2m_FAKE_ABSTRACTVIDEOOUTPUTBASE_H

#include <QObject>
#include "../../ifaces/abstractvideooutput.h"

namespace Kdem2m
{
namespace Fake
{
	class AbstractVideoOutput : public QObject, virtual public Ifaces::AbstractVideoOutput
	{
		Q_OBJECT
		public:
			AbstractVideoOutput( QObject* parent );
			virtual ~AbstractVideoOutput();

		public:
			virtual QObject* qobject() { return this; }
			virtual const QObject* qobject() const { return this; }

		private:
	};
}} //namespace Kdem2m::Fake

// vim: sw=4 ts=4 tw=80 noet
#endif // Kdem2m_FAKE_ABSTRACTVIDEOOUTPUTBASE_H
