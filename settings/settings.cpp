/*
    Copyright (C) 2014-2019 Harald Sitter <sitter@kde.org>

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

#include "settings.h"
#include "ui_settings.h"

#include <QApplication>

Settings::Settings(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Settings)
{
    ui->setupUi(this);
    ui->devicePreference->load();
    ui->backendSelection->load();

    // Force first tab regardless of what the UI file wants.
    ui->tabs->setCurrentIndex(0);

    connect(ui->buttonBox, &QDialogButtonBox::accepted,
            this, [=] {
        save();
        qApp->quit();
    });
    connect(ui->buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked,
            this, &Settings::save);
    connect(ui->buttonBox, &QDialogButtonBox::rejected,
            qApp, &QApplication::quit);
}

Settings::~Settings()
{
    delete ui;
}

void Settings::save()
{
    ui->devicePreference->save();
    ui->backendSelection->save();
}
