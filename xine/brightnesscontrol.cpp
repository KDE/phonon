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

#include "brightnesscontrol.h"
#include "mediaobject.h"
#include <kdebug.h>

namespace Phonon
{
namespace Xine
{

BrightnessControl::BrightnessControl(QObject *parent)
    : VideoEffect(1, parent)
    , m_brightness(100)
{
}

BrightnessControl::~BrightnessControl()
{
}

void BrightnessControl::setPath(VideoPath *path)
{
    VideoEffect::setPath(path);
    //TODO find output and set brightness
}

void BrightnessControl::setBrightness(int newBrightness)
{
    if (newBrightness < 0)
        newBrightness = 0;
    else if (newBrightness > 65535)
        newBrightness = 65535;
    if (m_brightness != newBrightness)
    {
        m_brightness = newBrightness;
        XineStream *s = stream();
        if (s) {
            kDebug(610) << m_brightness;
            s->setParam(XINE_PARAM_VO_BRIGHTNESS, m_brightness);
        }
    }
}

int BrightnessControl::upperBound() const
{
    return 65535;
}

int BrightnessControl::lowerBound() const
{
    return 0;
}

XineStream *BrightnessControl::stream()
{
    if (path() && path()->mediaObject()) {
        return &path()->mediaObject()->stream();
    }
    return 0;
}

} // namespace Xine
} // namespace Phonon

#include "brightnesscontrol.moc"
// vim: sw=4 ts=4
