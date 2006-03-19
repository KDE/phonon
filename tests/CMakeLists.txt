kde4_header()

add_subdirectory( fakebackend ) 

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


########### next target ###############

set(mediaobjecttest_SRCS mediaobjecttest.cpp )

kde4_automoc(${mediaobjecttest_SRCS})

if(KDE4_BUILD_TESTS)

kde4_add_executable(mediaobjecttest ${mediaobjecttest_SRCS})

target_link_libraries(mediaobjecttest  ${QT_AND_KDECORE_LIBS} )

endif(KDE4_BUILD_TESTS)

########### next target ###############

set(audiopathtest_SRCS audiopathtest.cpp )

kde4_automoc(${audiopathtest_SRCS})

if(KDE4_BUILD_TESTS)

kde4_add_executable(audiopathtest ${audiopathtest_SRCS})

target_link_libraries(audiopathtest  ${QT_AND_KDECORE_LIBS} )

endif(KDE4_BUILD_TESTS)

########### next target ###############

set(backendcapabilitiestest_SRCS backendcapabilitiestest.cpp )

kde4_automoc(${backendcapabilitiestest_SRCS})

if(KDE4_BUILD_TESTS)

kde4_add_executable(backendcapabilitiestest ${backendcapabilitiestest_SRCS})

target_link_libraries(backendcapabilitiestest  ${QT_AND_KDECORE_LIBS} )

endif(KDE4_BUILD_TESTS)

########### next target ###############

set(audiooutputtest_SRCS audiooutputtest.cpp )

kde4_automoc(${audiooutputtest_SRCS})

if(KDE4_BUILD_TESTS)

kde4_add_executable(audiooutputtest ${audiooutputtest_SRCS})

target_link_libraries(audiooutputtest  ${QT_AND_KDECORE_LIBS} )

endif(KDE4_BUILD_TESTS)

########### next target ###############

set(audiodataoutputtest_SRCS audiodataoutputtest.cpp )

kde4_automoc(${audiodataoutputtest_SRCS})

if(KDE4_BUILD_TESTS)

kde4_add_executable(audiodataoutputtest ${audiodataoutputtest_SRCS})

target_link_libraries(audiodataoutputtest  ${QT_AND_KDECORE_LIBS} )

endif(KDE4_BUILD_TESTS)

########### install files ###############


kde4_footer()



#original Makefile.am contents follow:

##    This file is part of the KDE libraries
##    Copyright (C) 2004-2005 Matthias Kretz <kretz@kde.org>
#
##    This library is free software; you can redistribute it and/or
##    modify it under the terms of the GNU Library General Public
##    License version 2 as published by the Free Software Foundation.
#
##    This library is distributed in the hope that it will be useful,
##    but WITHOUT ANY WARRANTY; without even the implied warranty of
##    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
##    Library General Public License for more details.
#
##    You should have received a copy of the GNU Library General Public License
##    along with this library; see the file COPYING.LIB.  If not, write to
##    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
##    Boston, MA 02111-1307, USA.
#
#SUBDIRS = . fakebackend
#INCLUDES = -I$(top_srcdir) $(all_includes)
#LDADD = ../libphononcore.la -lQtTest_debug
#AM_LDFLAGS = $(KDE_RPATH) $(all_libraries)
#
#METASOURCES = AUTO
#
#check_PROGRAMS = \
#				 mediaobjecttest \
#				 audiopathtest \
#				 backendcapabilitiestest \
#				 audiooutputtest \
#				 audiodataoutputtest
#				 #videopathtest
#
#mediaobjecttest_SOURCES = mediaobjecttest.cpp
#audiopathtest_SOURCES = audiopathtest.cpp
#audiooutputtest_SOURCES = audiooutputtest.cpp
#videopathtest_SOURCES = videopathtest.cpp
#backendcapabilitiestest_SOURCES = backendcapabilitiestest.cpp
#audiodataoutputtest_SOURCES = audiodataoutputtest.cpp
#
#TESTS = audiopathtest \
#		backendcapabilitiestest \
#		audiooutputtest \
#		audiodataoutputtest \
#		mediaobjecttest
#		#videopathtest
