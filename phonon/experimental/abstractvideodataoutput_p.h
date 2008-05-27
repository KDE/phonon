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
#ifndef PHONON_X_ABSTRACTVIDEODATAOUTPUT_P_H
#define PHONON_X_ABSTRACTVIDEODATAOUTPUT_P_H

#include "abstractvideodataoutput.h"
#include "../abstractvideooutput_p.h"

namespace Phonon
{
namespace Experimental
{

class AbstractVideoDataOutputPrivate : public Phonon::AbstractVideoOutputPrivate
{
    Q_DECLARE_PUBLIC(AbstractVideoDataOutput)
    protected:
        virtual bool aboutToDeleteBackendObject();
        virtual void createBackendObject();
        void setupBackendObject();

    private:
        bool isRunning;
};

} // namespace Experimental
} // namespace Phonon

#endif // PHONON_X_ABSTRACTVIDEODATAOUTPUT_P_H
