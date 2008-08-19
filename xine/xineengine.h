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

#ifndef XINEENGINE_H
#define XINEENGINE_H

#include <QtCore/QExplicitlySharedDataPointer>
#include <QtCore/QSharedData>

#include <xine.h>

namespace Phonon
{
namespace Xine
{

struct XineEngineData : public QSharedData
{
    XineEngineData();
    ~XineEngineData();

    xine_t *m_xine;
};

class XineEngine
{
    public:
        inline operator xine_t *() const { Q_ASSERT(d.data() && d->m_xine); return d->m_xine; }
        inline operator bool() const { return d; }
        inline bool operator==(const XineEngine &rhs) const { return d == rhs.d; }
        inline bool operator!=(const XineEngine &rhs) const { return d != rhs.d; }
        void create();

    private:
        QExplicitlySharedDataPointer<XineEngineData> d;
};

} // namespace Xine
} // namespace Phonon

#endif // XINEENGINE_H
