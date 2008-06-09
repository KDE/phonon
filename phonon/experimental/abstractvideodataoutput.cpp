/*  This file is part of the KDE project
    Copyright (C) 2008 Matthias Kretz <kretz@kde.org>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) version 3.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.

*/
#include "abstractvideodataoutput.h"
#include "abstractvideodataoutput_p.h"
#include "videodataoutputinterface.h"
#include "factory_p.h"

namespace Phonon
{
namespace Experimental
{

AbstractVideoDataOutput::AbstractVideoDataOutput()
    : AbstractVideoOutput(*new AbstractVideoDataOutputPrivate)
{
    K_D(AbstractVideoDataOutput);
    d->isRunning = false;
    d->allowedFormats << VideoFrame2::Format_RGB888;
}

AbstractVideoDataOutput::AbstractVideoDataOutput(AbstractVideoDataOutputPrivate &dd)
    : AbstractVideoOutput(dd)
{
}

AbstractVideoDataOutput::~AbstractVideoDataOutput()
{
    setRunning(false);
}

QSet<VideoFrame2::Format> AbstractVideoDataOutput::allowedFormats() const
{
    K_D(const AbstractVideoDataOutput);
    return d->allowedFormats;
}

void AbstractVideoDataOutput::setAllowedFormats(const QSet<VideoFrame2::Format> &allowedFormats)
{
    K_D(AbstractVideoDataOutput);
    d->allowedFormats = allowedFormats;
}

bool AbstractVideoDataOutput::isRunning() const
{
    K_D(const AbstractVideoDataOutput);
    return d->isRunning;
}

void AbstractVideoDataOutput::setRunning(bool running)
{
    K_D(AbstractVideoDataOutput);
    Iface<VideoDataOutputInterface> iface(d);
    if (iface) {
        if (running) {
            iface->setFrontendObject(this);
        } else {
            iface->setFrontendObject(0);
        }
    }
}

void AbstractVideoDataOutput::start()
{
    setRunning(true);
}

void AbstractVideoDataOutput::stop()
{
    setRunning(false);
}

bool AbstractVideoDataOutputPrivate::aboutToDeleteBackendObject()
{
    return AbstractVideoOutputPrivate::aboutToDeleteBackendObject();
}

void AbstractVideoDataOutputPrivate::setupBackendObject()
{
    Q_Q(AbstractVideoDataOutput);
    Q_ASSERT(m_backendObject);
    //AbstractVideoOutputPrivate::setupBackendObject();
    if (isRunning) {
        Iface<VideoDataOutputInterface> iface(this);
        if (iface) {
            iface->setFrontendObject(q);
        }
    }
}

void AbstractVideoDataOutputPrivate::createBackendObject()
{
    if (m_backendObject)
        return;
    //Q_Q(AbstractVideoDataOutput);
    m_backendObject = Factory::createVideoDataOutput(0);
    if (m_backendObject) {
        setupBackendObject();
    }
}

} // namespace Experimental
} // namespace Phonon
