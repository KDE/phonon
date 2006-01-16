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
#ifndef Phonon_FAKE_MEDIAOBJECT_H
#define Phonon_FAKE_MEDIAOBJECT_H

#include "abstractmediaproducer.h"
#include "../../ifaces/mediaobject.h"
#include <kurl.h>

class KURL;

namespace Phonon
{
namespace Fake
{
	class MediaObject : virtual public AbstractMediaProducer, virtual public Ifaces::MediaObject
	{
		Q_OBJECT
		public:
			MediaObject( QObject* parent );
			virtual ~MediaObject();
			virtual KURL url() const;
			virtual long totalTime() const;
			//virtual long remainingTime() const;
			virtual long aboutToFinishTime() const;
			virtual void setUrl( const KURL& url );
			virtual long setAboutToFinishTime( long newAboutToFinishTime );

			virtual void play();
			virtual void pause();
			virtual void seek( long time );

		public slots:
			virtual void stop();

		signals:
			void finished();
			void aboutToFinish( long msec );
			void length( long length );

		private slots:
			void aboutToFinishTimeout();

		private:
			KURL m_url;
			long m_aboutToFinishTime;
			QTimer* m_aboutToFinishTimer;
			QTimer* m_finishTimer;
	};
}} //namespace Phonon::Fake

// vim: sw=4 ts=4 tw=80 noet
#endif // Phonon_FAKE_MEDIAOBJECT_H
