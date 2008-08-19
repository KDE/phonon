/*  This file is part of the KDE project
    Copyright (C) 2007 Matthias Kretz <kretz@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2
    as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
    02110-1301, USA.

*/

#ifndef PHONON_XINEOPTIONS_H
#define PHONON_XINEOPTIONS_H

#include "ui_xineoptions.h"
#define KDE3_SUPPORT
#include <kcmodule.h>
#undef KDE3_SUPPORT
#include <kconfig.h>

class XineOptions : public KCModule, private Ui::XineOptions
{
    Q_OBJECT
    public:
        XineOptions(QWidget *parent, const QVariantList &args);

        void load();
        void save();
        void defaults();

    private:
        KSharedConfigPtr m_config;
        bool m_noDeinterlace;
};

#endif // PHONON_XINEOPTIONS_H
