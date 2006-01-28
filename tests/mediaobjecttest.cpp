/*  This file is part of the KDE project
    Copyright (C) 2006 Matthias Kretz <kretz@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2
    as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
    02110-1301, USA.

*/

#include "mediaobjecttest.h"
#include <cstdlib>
#include <qtest_kde.h>

using namespace Phonon;

Q_DECLARE_METATYPE( Phonon::State )

void MediaObjectTest::initTestCase()
{
	qRegisterMetaType<Phonon::State>( "Phonon::State" );

	m_url = getenv( "PHONON_TESTURL" );
	m_media = new MediaObject( this );
	m_spy = new QSignalSpy( m_media, SIGNAL( stateChanged( Phonon::State, Phonon::State ) ) );
	QVERIFY( m_spy->isValid() );
	m_spy->clear();
}

void MediaObjectTest::setMedia()
{
	QVERIFY( m_media->url().isEmpty() );
	QCOMPARE( m_media->state(), Phonon::LoadingState );
	QCOMPARE( m_spy->count(), 0 );
	m_media->setUrl( m_url );
	QCOMPARE( m_url, m_media->url() );
	int emits = m_spy->count();
	Phonon::State s = m_media->state();
	if( s == Phonon::LoadingState )
	{
		// still in LoadingState, there should be no state change
		QCOMPARE( emits, 0 );
		while( m_spy->count() == 0 )
			QCoreApplication::processEvents();
		emits = m_spy->count();
		s = m_media->state();
	}
	if( s != Phonon::LoadingState )
	{
		// there should exactly be one state change
		QCOMPARE( emits, 1 );
		QList<QVariant> args = m_spy->takeFirst();
		Phonon::State newstate = qvariant_cast<Phonon::State>( args.at( 0 ) );
		Phonon::State oldstate = qvariant_cast<Phonon::State>( args.at( 1 ) );

		QCOMPARE( Phonon::LoadingState, oldstate );
		QCOMPARE( s, newstate );
		if( Phonon::ErrorState == s )
			QFAIL( "Loading the URL put the MediaObject into the ErrorState. Check that PHONON_TESTURL is set to a valid URL." );
		QCOMPARE( Phonon::StoppedState, s );
		QCOMPARE( m_spy->count(), 0 );
	}
	else
	{
		QFAIL( "Still in LoadingState after a stateChange signal was emitted. Cannot go on." );
	}
}

void MediaObjectTest::stopToStop()
{
	QCOMPARE( m_spy->count(), 0 );
	QCOMPARE( m_media->state(), Phonon::StoppedState );
	m_media->stop();
	QCOMPARE( m_spy->count(), 0 );
	QCOMPARE( m_media->state(), Phonon::StoppedState );
}

void MediaObjectTest::stopToPause()
{
	QCOMPARE( m_spy->count(), 0 );
	QCOMPARE( m_media->state(), Phonon::StoppedState );
	m_media->pause();
	QCOMPARE( m_spy->count(), 0 );
	QCOMPARE( m_media->state(), Phonon::StoppedState );
}

void MediaObjectTest::stopToPlay()
{
	QCOMPARE( m_spy->count(), 0 );
	QCOMPARE( m_media->state(), Phonon::StoppedState );
	m_media->play();
	QCOMPARE( m_spy->count(), 1 );
	QList<QVariant> args = m_spy->takeFirst();
	Phonon::State newstate = qvariant_cast<Phonon::State>( args.at( 0 ) );
	Phonon::State oldstate = qvariant_cast<Phonon::State>( args.at( 1 ) );
	QCOMPARE( oldstate, Phonon::StoppedState );
	QCOMPARE( newstate, m_media->state() );
	if( newstate == Phonon::BufferingState )
	{
		QCOMPARE( m_spy->count(), 0 );
		while( m_spy->count() == 0 )
			QCoreApplication::processEvents();
		QCOMPARE( m_spy->count(), 1 );

		args = m_spy->takeFirst();
		newstate = qvariant_cast<Phonon::State>( args.at( 0 ) );
		oldstate = qvariant_cast<Phonon::State>( args.at( 1 ) );
		QCOMPARE( oldstate, Phonon::BufferingState );
	}
	QCOMPARE( newstate, Phonon::PlayingState );
	QCOMPARE( m_media->state(), Phonon::PlayingState );
	m_media->stop();
	QCOMPARE( m_spy->count(), 1 );
	args = m_spy->takeFirst();
	newstate = qvariant_cast<Phonon::State>( args.at( 0 ) );
	oldstate = qvariant_cast<Phonon::State>( args.at( 1 ) );
	QCOMPARE( oldstate, Phonon::PlayingState );
	QCOMPARE( newstate, Phonon::StoppedState );
	QCOMPARE( m_media->state(), Phonon::StoppedState );
}

void MediaObjectTest::playToPlay()
{
	QCOMPARE( m_spy->count(), 0 );
	QCOMPARE( m_media->state(), Phonon::StoppedState );
	m_media->play();
	QCOMPARE( m_spy->count(), 1 );
	QList<QVariant> args = m_spy->takeFirst();
	Phonon::State newstate = qvariant_cast<Phonon::State>( args.at( 0 ) );
	Phonon::State oldstate = qvariant_cast<Phonon::State>( args.at( 1 ) );
	QCOMPARE( oldstate, Phonon::StoppedState );
	QCOMPARE( newstate, m_media->state() );
	if( newstate == Phonon::BufferingState )
	{
		QCOMPARE( m_spy->count(), 0 );
		while( m_spy->count() == 0 )
			QCoreApplication::processEvents();
		QCOMPARE( m_spy->count(), 1 );

		args = m_spy->takeFirst();
		newstate = qvariant_cast<Phonon::State>( args.at( 0 ) );
		oldstate = qvariant_cast<Phonon::State>( args.at( 1 ) );
		QCOMPARE( oldstate, Phonon::BufferingState );
	}
	QCOMPARE( newstate, Phonon::PlayingState );
	QCOMPARE( m_media->state(), Phonon::PlayingState );
	m_media->play();
	QCOMPARE( m_spy->count(), 0 );
	QCOMPARE( m_media->state(), Phonon::PlayingState );
	m_media->stop();
	QCOMPARE( m_spy->count(), 1 );
	args = m_spy->takeFirst();
	newstate = qvariant_cast<Phonon::State>( args.at( 0 ) );
	oldstate = qvariant_cast<Phonon::State>( args.at( 1 ) );
	QCOMPARE( oldstate, Phonon::PlayingState );
	QCOMPARE( newstate, Phonon::StoppedState );
	QCOMPARE( m_media->state(), Phonon::StoppedState );
}

void MediaObjectTest::playToPause()
{
	QCOMPARE( m_spy->count(), 0 );
	QCOMPARE( m_media->state(), Phonon::StoppedState );
	m_media->play();
	QCOMPARE( m_spy->count(), 1 );
	QList<QVariant> args = m_spy->takeFirst();
	Phonon::State newstate = qvariant_cast<Phonon::State>( args.at( 0 ) );
	Phonon::State oldstate = qvariant_cast<Phonon::State>( args.at( 1 ) );
	QCOMPARE( oldstate, Phonon::StoppedState );
	QCOMPARE( newstate, m_media->state() );
	if( newstate == Phonon::BufferingState )
	{
		QCOMPARE( m_spy->count(), 0 );
		while( m_spy->count() == 0 )
			QCoreApplication::processEvents();
		QCOMPARE( m_spy->count(), 1 );

		args = m_spy->takeFirst();
		newstate = qvariant_cast<Phonon::State>( args.at( 0 ) );
		oldstate = qvariant_cast<Phonon::State>( args.at( 1 ) );
		QCOMPARE( oldstate, Phonon::BufferingState );
	}
	QCOMPARE( newstate, Phonon::PlayingState );
	QCOMPARE( m_media->state(), Phonon::PlayingState );
	m_media->pause();
	QCOMPARE( m_spy->count(), 1 );
	args = m_spy->takeFirst();
	newstate = qvariant_cast<Phonon::State>( args.at( 0 ) );
	oldstate = qvariant_cast<Phonon::State>( args.at( 1 ) );
	QCOMPARE( oldstate, Phonon::PlayingState );
	QCOMPARE( newstate, Phonon::PausedState );
	QCOMPARE( m_media->state(), Phonon::PausedState );
	m_media->stop();
	QCOMPARE( m_spy->count(), 1 );
	args = m_spy->takeFirst();
	newstate = qvariant_cast<Phonon::State>( args.at( 0 ) );
	oldstate = qvariant_cast<Phonon::State>( args.at( 1 ) );
	QCOMPARE( oldstate, Phonon::PausedState );
	QCOMPARE( newstate, Phonon::StoppedState );
	QCOMPARE( m_media->state(), Phonon::StoppedState );
}

void MediaObjectTest::playToStop()
{
}

void MediaObjectTest::pauseToPause()
{
}

void MediaObjectTest::pauseToPlay()
{
}

void MediaObjectTest::pauseToStop()
{
}

void MediaObjectTest::cleanupTestCase()
{
}

QTEST_KDEMAIN( MediaObjectTest, NoGUI )
#include "mediaobjecttest.moc"
// vim: sw=4 ts=4 noet
