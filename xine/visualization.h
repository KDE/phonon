/*  This file is part of the KDE project
    Copyright (C) 2006 Matthias Kretz <kretz@kde.org>

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

#ifndef PHONON_XINE_VISUALIZATION_H
#define PHONON_XINE_VISUALIZATION_H

#include <QObject>
#include "sinknode.h"
#include "sourcenode.h"

namespace Phonon
{
namespace Xine
{

class Visualization : public QObject, public SinkNode, public SourceNode
{
    Q_OBJECT
    Q_INTERFACES(Phonon::Xine::SinkNode Phonon::Xine::SourceNode)
    public:
        Visualization(QObject *parent = 0);

        MediaStreamTypes inputMediaStreamTypes() const { return Phonon::Xine::Audio; }
        MediaStreamTypes outputMediaStreamTypes() const { return Phonon::Xine::Video; }

    public slots:
        int visualization() const;
        void setVisualization(int newVisualization);

    private:
        int m_visualization;
};

}} //namespace Phonon::Xine

#endif // PHONON_XINE_VISUALIZATION_H
