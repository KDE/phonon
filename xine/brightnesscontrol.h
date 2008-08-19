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

#ifndef BRIGHTNESSCONTROL_H
#define BRIGHTNESSCONTROL_H

#include "videoeffect.h"
#include <xine.h>
#include "xinestream.h"

namespace Phonon
{
namespace Xine
{

class BrightnessControl : public VideoEffect
{
    Q_OBJECT
    public:
        BrightnessControl(QObject *parent = 0);
        ~BrightnessControl();

        Q_INVOKABLE int brightness() const { return m_brightness; }
        Q_INVOKABLE void setBrightness(int);

        Q_INVOKABLE int upperBound() const;
        Q_INVOKABLE int lowerBound() const;

        virtual void setPath(VideoPath *);

    private:
        XineStream *stream();

        int m_brightness;
};

} // namespace Xine
} // namespace Phonon

#endif // BRIGHTNESSCONTROL_H

// vim: sw=4 ts=4 sts=4 et
