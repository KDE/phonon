/*  This file is part of the KDE project
    Copyright (C) 2006 Matthias Kretz <kretz@kde.org>

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

#include "visualization.h"
#include "visualization_p.h"
#include "../objectdescription.h"
#include "../path.h"
#include "factory_p.h"

#define PHONON_CLASSNAME Visualization

namespace Phonon
{
namespace Experimental
{
PHONON_OBJECT_IMPL

Visualization::~Visualization()
{
}

VisualizationDescription Visualization::visualization() const
{
    P_D(const Visualization);
    if (!d->m_backendObject) {
        return d->description;
    }
    int index;
    BACKEND_GET(int, index, "visualization");
    return VisualizationDescription::fromIndex(index);
}

void Visualization::setVisualization(const VisualizationDescription &newVisualization)
{
    P_D(Visualization);
    d->description = newVisualization;
    if (k_ptr->backendObject()) {
        BACKEND_CALL1("setVisualization", int, newVisualization.index());
    }
}

/*
bool Visualization::hasParameterWidget() const
{
    P_D(const Visualization);
    if (d->m_backendObject)
    {
        bool ret;
        BACKEND_GET(bool, ret, "hasParameterWidget");
        return ret;
    }
    return false;
}

QWidget *Visualization::createParameterWidget(QWidget *parent)
{
    P_D(Visualization);
    if (k_ptr->backendObject())
    {
        QWidget *ret;
        BACKEND_GET1(QWidget *, ret, "createParameterWidget", QWidget *, parent);
        return ret;
    }
    return 0;
}
*/

void VisualizationPrivate::phononObjectDestroyed(MediaNodePrivate *bp)
{
    Q_UNUSED(bp);
    // this method is called from Phonon::MediaNodePrivate::~MediaNodePrivate(), meaning the AudioEffect
    // dtor has already been called and the private class is down to MediaNodePrivate
    /*
    Q_ASSERT(bp);
    if (audioPath->k_ptr == bp)
    {
        pBACKEND_CALL1("setAudioPath", QObject *, static_cast<QObject *>(0));
        audioPath = 0;
    }
    else if (videoOutput->k_ptr == bp)
    {
        pBACKEND_CALL1("setVideoOutput", QObject *, static_cast<QObject *>(0));
        videoOutput = 0;
    }
    */
}

bool VisualizationPrivate::aboutToDeleteBackendObject()
{
    return true;
}

void VisualizationPrivate::setupBackendObject()
{
    Q_ASSERT(m_backendObject);

    pBACKEND_CALL1("setVisualization", int, description.index());
    /*
    if (audioPath)
        pBACKEND_CALL1("setAudioPath", QObject *, audioPath->k_ptr->backendObject());
    if (videoOutput)
        pBACKEND_CALL1("setVideoOutput", QObject *, videoOutput->k_ptr->backendObject());
        */
}

} // namespace Experimental
} // namespace Phonon

#undef PHONON_CLASSNAME
// vim: sw=4 ts=4
