/*
    Copyright (C) 2004-2007 Matthias Kretz <kretz@kde.org>
    Copyright (C) 2011-2019 Harald Sitter <sitter@kde.org>

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

#include "backendselection.h"

#include <QList>
#include <QStringList>
#include <QListWidget>

#include <QDebug>
#include <QDesktopServices>
#include <QDir>
#include <QJsonArray>
#include <QJsonObject>
#include <QPluginLoader>
#include <QSettings>
#include <QUrl>

BackendSelection::BackendSelection(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);

    m_down->setIcon(QIcon::fromTheme("go-down"));
    m_up->setIcon(QIcon::fromTheme("go-up"));
    m_comment->setWordWrap(true);

    m_emptyPage = stackedWidget->addWidget(new QWidget());

    connect(m_select, &QListWidget::itemSelectionChanged,
            this, &BackendSelection::selectionChanged);
    connect(m_up, &QToolButton::clicked,
            this, &BackendSelection::up);
    connect(m_down, &QToolButton::clicked,
            this, &BackendSelection::down);

    connect(m_website, &QLabel::linkActivated,
            this, [=](const QString &url) { QDesktopServices::openUrl(QUrl(url)); });
}

void BackendSelection::load()
{
    // NOTE: for phonon5 this should move into the library in some form.
    m_backends.clear();

    const auto backends = Phonon::Factory::findBackends();

    m_select->clear();

    for (const auto &bd : backends) {
        m_select->addItem(bd.name);
        m_backends.insert(bd.name, bd);
    }
    m_select->setItemSelected(m_select->item(0), true);
}

void BackendSelection::save()
{
    QSettings settings("kde.org", "libphonon");
    settings.beginWriteArray("Backends", m_select->count());
    for (int i = 0; i < m_select->count(); ++i) {
        settings.setArrayIndex(i);
        const QListWidgetItem *item = m_select->item(i);
        const auto backend = m_backends.value(item->text());
        settings.setValue("iid", backend.iid);
    }
    settings.endArray();
    settings.sync();
}

void BackendSelection::selectionChanged()
{
    if (m_select->selectedItems().isEmpty()) {
        m_up->setEnabled(false);
        m_down->setEnabled(false);
        return;
    }

    const QListWidgetItem *const item = m_select->selectedItems().first();
    m_up->setEnabled(m_select->row(item) > 0);
    m_down->setEnabled(m_select->row(item) < m_select->count() - 1);
    const auto backend = m_backends[item->text()];
    m_icon->setPixmap(QIcon::fromTheme(backend.icon).pixmap(128, 128));
    m_name->setText(backend.name);
    m_website->setText(QString("<a href=\"%1\">%1</a>").arg(backend.website));
    m_version->setText(backend.version);
}

void BackendSelection::up()
{
    const QList<QListWidgetItem *> selectedList = m_select->selectedItems();
    for (const QListWidgetItem *selected : selectedList) {
        const int row = m_select->row(selected);
        if (row <= 0) {
            continue;
        }
        QListWidgetItem *taken = m_select->takeItem(row - 1);
        m_select->insertItem(row, taken);
        selectionChanged();
    }
}

void BackendSelection::down()
{
    const QList<QListWidgetItem *> selectedList = m_select->selectedItems();
    for (const QListWidgetItem *selected : selectedList) {
        const int row = m_select->row(selected);
        if (row + 1 >= m_select->count()) {
            continue;
        }
        QListWidgetItem *taken = m_select->takeItem(row + 1);
        m_select->insertItem(row, taken);
        selectionChanged();
    }
}
