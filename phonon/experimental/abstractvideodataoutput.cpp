/*  This file is part of the KDE project
    Copyright (C) 2008 Matthias Kretz <kretz@kde.org>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of
    the License or (at your option) version 3 or any later version
    accepted by the membership of KDE e.V. (or its successor approved
    by the membership of KDE e.V.), Nokia Corporation (or its successors,
    if any) and the KDE Free Qt Foundation, which shall act as a proxy
    defined in Section 14 of version 3 of the license.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

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
    P_D(AbstractVideoDataOutput);
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
    P_D(const AbstractVideoDataOutput);
    return d->allowedFormats;
}

void AbstractVideoDataOutput::setAllowedFormats(const QSet<VideoFrame2::Format> &allowedFormats)
{
    P_D(AbstractVideoDataOutput);
    d->allowedFormats = allowedFormats;
}

bool AbstractVideoDataOutput::isRunning() const
{
    P_D(const AbstractVideoDataOutput);
    return d->isRunning;
}

void AbstractVideoDataOutput::setRunning(bool running)
{
    P_D(AbstractVideoDataOutput);
    d->isRunning = running;
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
    P_Q(AbstractVideoDataOutput);
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
    //P_Q(AbstractVideoDataOutput);
    m_backendObject = Factory::createVideoDataOutput(0);
    if (m_backendObject) {
        setupBackendObject();
    }
}

} // namespace Experimental
} // namespace Phonon
