kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


########### next target ###############

set(seekslider_SRCS seekslider.cpp )

kde4_automoc(${seekslider_SRCS})

if(KDE4_BUILD_TESTS)

kde4_add_executable(seekslider ${seekslider_SRCS})

target_link_libraries(seekslider  ${QT_AND_KDECORE_LIBS} )

endif(KDE4_BUILD_TESTS)

########### next target ###############

set(guitest_SRCS main.cpp testwidget.cpp )

kde4_automoc(${guitest_SRCS})

if(KDE4_BUILD_TESTS)

kde4_add_executable(guitest ${guitest_SRCS})

target_link_libraries(guitest  ${QT_AND_KDECORE_LIBS} )

endif(KDE4_BUILD_TESTS)

########### next target ###############

set(mediaplayer_SRCS mediaplayer.cpp )

kde4_automoc(${mediaplayer_SRCS})

if(KDE4_BUILD_TESTS)

kde4_add_executable(mediaplayer ${mediaplayer_SRCS})

target_link_libraries(mediaplayer  ${QT_AND_KDECORE_LIBS} )

endif(KDE4_BUILD_TESTS)

########### install files ###############


kde4_footer()



#original Makefile.am contents follow:

##    This file is part of the KDE libraries
##    Copyright (C) 2006 Matthias Kretz <kretz@kde.org>
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
#INCLUDES = -I$(top_srcdir) $(all_includes)
#LDADD = ../libphononui.la -lQtTest_debug
#AM_LDFLAGS = $(KDE_RPATH) $(all_libraries)
#
#METASOURCES = AUTO
#
#check_PROGRAMS = seekslider guitest mediaplayer
#
#seekslider_SOURCES = seekslider.cpp
#guitest_SOURCES = main.cpp testwidget.cpp
#mediaplayer_SOURCES = mediaplayer.cpp
#
#TESTS = seekslider
