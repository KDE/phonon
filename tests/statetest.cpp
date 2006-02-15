/*  This file is part of the KDE project
    Copyright (C) 2004-2005 Matthias Kretz <kretz@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License version 2 as published by the Free Software Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.

*/

#include "statetest.h"
#include <QTimer>
#include <qtest_kde.h>
#include <kurl.h>
#include <phonon/mediaobject.h>
#include <cstdlib>

using namespace Phonon;

void StateTester::initTestCase()
{
	m_url = new KUrl( getenv( "PHONON_TESTURL" ) );
	if( !m_url->isValid() )
		QFAIL( "You need to set PHONON_TESTURL to a valid URL" );
}

void StateTester::run()
{
	/*
	check for correct states:

	- after construction:
	  LoadingState -> ( StoppedState, ErrorState ), StoppedState, ErrorState

	- play()
	  ( StoppedState, PausedState ) -> PlayingState

	- when playing:
	  PlayingState -> BufferingState -> ( PlayingState, ErrorState )

	- pause()
	  ( StoppedState, PlayingState, BufferingState ) -> PausedState

	- stop()
	  ( PlayingState, PausedState, BufferingState ) -> StoppedState
	*/

	//f = Factory::self();
	//kdDebug() << "using backend: " << f->backendName() <<
		//"\n Comment: " << f->backendComment() <<
		//"\n Version: " << f->backendVersion() << endl;

	//c = f->createChannel( "teststates" );

	kdDebug() << "loading " << *m_url << endl;
	kdDebug() << this << endl;
	player = new MediaObject( this );
	player->setUrl( *m_url );
	connect( player, SIGNAL( stateChanged( Phonon::State, Phonon::State ) ),
			SLOT( stateChanged( Phonon::State, Phonon::State ) ) );
	connect( player, SIGNAL( finished() ), kapp, SLOT( quit() ) );

	if( player->state() == Phonon::LoadingState )
		kdDebug() << "wait until Phonon finished LoadingState" << endl;
	else if( player->state() == Phonon::StoppedState )
		testplaying();
	else if( player->state() == Phonon::ErrorState )
	{
		kdDebug() << "could not load media. exiting." << endl;
		exit( 0 );
	}
}

void StateTester::stateChanged( Phonon::State newstate, Phonon::State oldstate )
{
	kdDebug() << "stateChanged( new = " << newstate << ", old = " << oldstate << " )" << endl;
	switch( oldstate )
	{
		case Phonon::LoadingState:
			switch( newstate )
			{
				case Phonon::ErrorState:
					return;
				case Phonon::StoppedState:
					testplaying();
					return;
				default:
					break;
			}
			break;
		case Phonon::StoppedState:
			switch( newstate )
			{
				case Phonon::PlayingState:
				case Phonon::PausedState:
					return;
				default:
					break;
			}
			break;
		case Phonon::PlayingState:
			switch( newstate )
			{
				case Phonon::BufferingState:
					//testbuffering();
				case Phonon::PausedState:
				case Phonon::StoppedState:
					return;
				default:
					break;
			}
			break;
		case Phonon::BufferingState:
			switch( newstate )
			{
				case Phonon::PlayingState:
				case Phonon::StoppedState:
				case Phonon::PausedState:
				case Phonon::ErrorState:
					return;
				default:
					break;
			}
			break;
		case Phonon::PausedState:
			switch( newstate )
			{
				case Phonon::PlayingState:
				case Phonon::StoppedState:
					return;
				default:
					break;
			}
			break;
		case Phonon::ErrorState:
			break;
	}

	wrongStateChange();
}

void StateTester::testplaying()
{
	player->play();
	if( player->state() == Phonon::StoppedState )
	{
		kdDebug() << "could not play media. exiting." << endl;
		exit( 0 );
	}
	else if( player->state() == Phonon::PlayingState )
	{
		player->pause();
		if( player->state() != Phonon::PausedState )
			wrongStateChange();
		player->play();
		if( player->state() != Phonon::PlayingState )
			wrongStateChange();
		player->stop();
		if( player->state() != Phonon::StoppedState )
			wrongStateChange();
		player->play();
		if( player->state() != Phonon::PlayingState )
			wrongStateChange();
		player->pause();
		if( player->state() != Phonon::PausedState )
			wrongStateChange();
		player->stop();
		if( player->state() != Phonon::StoppedState )
			wrongStateChange();
		kdDebug() << "success! playing the last 1/5 of the file now and quit on the finished signal" << endl;
		player->play();
		player->seek( player->totalTime() * 4 / 5 );
	}
}

void StateTester::wrongStateChange()
{
	kdError() << "wrong state change in backend!\n" << kdBacktrace() << endl;
	exit( 1 );
}

void StateTester::cleanupTestCase()
{
}

QTEST_KDEMAIN( StateTester, NoGUI )
#include "statetest.moc"

// vim: sw=4 ts=4 noet
