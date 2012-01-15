/*  This file is part of the KDE project
    Copyright (C) 2008 Matthias Kretz <kretz@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), Nokia Corporation
    (or its successors, if any) and the KDE Free Qt Foundation, which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "abstractaudiodataoutput.h"
#include "abstractaudiodataoutput_p.h"
#include "audiodataoutputinterface.h"
#include "factory_p.h"
#include "../phonondefs_p.h"

namespace Phonon
{
namespace Experimental
{

AbstractAudioDataOutput::AbstractAudioDataOutput()
    : MediaNode(*new AbstractAudioDataOutputPrivate)
{
    P_D(AbstractAudioDataOutput);
    d->isRunning = false;
    d->allowedFormats << AudioFormat();
}

AbstractAudioDataOutput::~AbstractAudioDataOutput()
{
    setRunning(false);
}

QSet<AudioFormat> AbstractAudioDataOutput::allowedFormats() const
{
    P_D(const AbstractAudioDataOutput);
    return d->allowedFormats;
}

void AbstractAudioDataOutput::setAllowedFormats(const QSet<AudioFormat> &allowedFormats)
{
    P_D(AbstractAudioDataOutput);
    d->allowedFormats = allowedFormats;
}

bool AbstractAudioDataOutput::isRunning() const
{
    P_D(const AbstractAudioDataOutput);
    return d->isRunning;
}

void AbstractAudioDataOutput::setRunning(bool running)
{
    P_D(AbstractAudioDataOutput);
    Iface<AudioDataOutputInterface> iface(d);
    if (iface) {
        if (running) {
            iface->setFrontendObject(this);
        } else {
            iface->setFrontendObject(0);
        }
    }
}

void AbstractAudioDataOutput::start()
{
    setRunning(true);
}

void AbstractAudioDataOutput::stop()
{
    setRunning(false);
}

void AbstractAudioDataOutputPrivate::setupBackendObject()
{
    P_Q(AbstractAudioDataOutput);
    Q_ASSERT(m_backendObject);
    //AbstractAudioOutputPrivate::setupBackendObject();
    if (isRunning) {
        Iface<AudioDataOutputInterface> iface(this);
        if (iface) {
            iface->setFrontendObject(q);
        }
    }
}

void AbstractAudioDataOutputPrivate::createBackendObject()
{
    if (m_backendObject)
        return;
    //P_Q(AbstractAudioDataOutput);
    m_backendObject = Factory::createAudioDataOutput(0);
    if (m_backendObject) {
        setupBackendObject();
    }
}

} // namespace Experimental
} // namespace Phonon
