/*  This file is part of the KDE project
    Copyright (C) 2006 Tim Beaulen <tbscope@gmail.com>
    Copyright (C) 2006-2007 Matthias Kretz <kretz@kde.org>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.

*/
#ifndef Phonon_XINE_ABSTRACTAUDIOOUTPUTBASE_H
#define Phonon_XINE_ABSTRACTAUDIOOUTPUTBASE_H

#include <QObject>
#include <QList>
#include "sinknode.h"

namespace Phonon
{
namespace Xine
{
class AbstractAudioOutput : public QObject, public SinkNode
{
    Q_OBJECT
    Q_INTERFACES(Phonon::Xine::SinkNode)
    public:
        AbstractAudioOutput(SinkNodeXT *, QObject *parent);
        virtual ~AbstractAudioOutput();

        MediaStreamTypes inputMediaStreamTypes() const { return Phonon::Xine::Audio; }
};
}} //namespace Phonon::Xine

// vim: sw=4 ts=4 tw=80
#endif // Phonon_XINE_ABSTRACTAUDIOOUTPUTBASE_H
