/*  This file is part of the KDE project
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

#include "deinterlacefilter.h"
#include "mediaobject.h"
#include <kdebug.h>

namespace Phonon
{
namespace Xine
{

DeinterlaceFilter::DeinterlaceFilter(QObject *parent)
    : VideoEffect(2, parent)
{
}

DeinterlaceFilter::~DeinterlaceFilter()
{
    if (path()) {
        MediaObject *p = path()->mediaObject();
        if (p) {
            XineStream &xs = p->stream();
            xs.setParam(XINE_PARAM_VO_DEINTERLACE, false);
        }
    }
}

void DeinterlaceFilter::setPath(VideoPath *_path)
{
    if (path()) { // remove from old stream
        MediaObject *p = path()->mediaObject();
        if (p) {
            XineStream &xs = p->stream();
            xs.setParam(XINE_PARAM_VO_DEINTERLACE, false);
        }
    }
    VideoEffect::setPath(_path);
    if (path()) {
        MediaObject *p = path()->mediaObject();
        if (p) {
            XineStream &xs = p->stream();
            xs.setParam(XINE_PARAM_VO_DEINTERLACE, true);
        }
    }
}

} // namespace Xine
} // namespace Phonon

#include "deinterlacefilter.moc"
// vim: sw=4 ts=4
