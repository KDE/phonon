/*
    Copyright (C) 2011 Harald Sitter <sitter@kde.org>

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

/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Designer of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "videoplayertaskmenu.h"

#include <QtDesigner/QDesignerFormWindowInterface>
#include <QtDesigner/QDesignerFormWindowCursorInterface>
#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QExtensionManager>

#include <phonon/backendcapabilities.h>
#include <phonon/mediaobject.h>
#include <phonon/videoplayer.h>

#include <QPlainTextEdit>
#include <QDialogButtonBox>
#include <QAction>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QMessageBox>

// -----------------  MimeTypeDialog: Display mime types in scrollable text

class MimeTypeDialog : public QDialog {
    Q_DISABLE_COPY(MimeTypeDialog)
public:
    explicit MimeTypeDialog(QWidget *parent = 0);

    void setMimeTypes(const QStringList &);

private:
    QPlainTextEdit *m_plainTextEdit;
};

MimeTypeDialog::MimeTypeDialog(QWidget *parent) :
    QDialog(parent),
    m_plainTextEdit(new QPlainTextEdit)
{
    setModal(true);
    setWindowTitle(VideoPlayerTaskMenu::tr("Available Mime Types"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    QVBoxLayout *layout = new QVBoxLayout;
    m_plainTextEdit->setReadOnly(true);
    layout->addWidget(m_plainTextEdit);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    layout->addWidget(buttonBox);

    setLayout(layout);
}

void MimeTypeDialog::setMimeTypes(const QStringList &l)
{
    m_plainTextEdit->setPlainText(l.join(QString(1, QLatin1Char('\n'))));
}

// ----------------- VideoPlayerTaskMenu
VideoPlayerTaskMenu::VideoPlayerTaskMenu(Phonon::VideoPlayer *object, QObject *parent) :
    QObject(parent),
    m_widget(object),
    m_displayMimeTypesAction(new QAction(tr("Display supported mime types..."), this)),
    m_loadAction(new QAction(tr("Load..."), this)),
    m_playAction(new QAction(tr("Play"), this)),
    m_pauseAction(new QAction(tr("Pause"), this)),
    m_stopAction(new QAction(tr("Stop"), this))
{
    m_taskActions << m_displayMimeTypesAction << m_loadAction << m_playAction << m_pauseAction << m_stopAction;

    connect(m_widget->mediaObject(), SIGNAL(stateChanged(Phonon::State,Phonon::State)), this, SLOT(mediaObjectStateChanged(Phonon::State,Phonon::State)));
    connect(m_displayMimeTypesAction, SIGNAL(triggered()), this, SLOT(slotMimeTypes()));
    connect(m_loadAction, SIGNAL(triggered()), this, SLOT(slotLoad()));
    connect(m_playAction, SIGNAL(triggered()), object, SLOT(play()));
    connect(m_pauseAction, SIGNAL(triggered()), object, SLOT(pause()));
    connect(m_stopAction, SIGNAL(triggered()), object, SLOT(stop()));
}

QList<QAction*> VideoPlayerTaskMenu::taskActions() const
{
    const bool isPlaying = m_widget->isPlaying();
    const bool isPaused = m_widget->isPlaying();
    m_loadAction->setEnabled(!isPlaying && !isPaused);
    m_playAction->setEnabled(!isPlaying);
    m_pauseAction->setEnabled(isPlaying);
    m_stopAction->setEnabled(isPlaying || isPaused);
    return m_taskActions;
}

void VideoPlayerTaskMenu::slotMimeTypes()
{
    MimeTypeDialog mimeTypeDialog(m_widget->window());
    mimeTypeDialog.setMimeTypes(Phonon::BackendCapabilities::availableMimeTypes());
    mimeTypeDialog.exec();
}

void VideoPlayerTaskMenu::slotLoad()
{
    const QUrl fileName = QUrl::fromLocalFile(QFileDialog::getOpenFileName(m_widget->window(), tr("Choose Video Player Media Source")));
    if (fileName.isEmpty())
        return;
    m_widget->load(Phonon::MediaSource(fileName));

}

void VideoPlayerTaskMenu::mediaObjectStateChanged(Phonon::State newstate, Phonon::State /* oldstate */)
{
    if (newstate == Phonon::ErrorState) {
        const QString msg = tr("An error has occurred in '%1': %2").arg(m_widget->objectName(), m_widget->mediaObject()->errorString());
        QMessageBox::warning(m_widget->window(), tr("Video Player Error"), msg);
    }
}

// ----------------- VideoPlayerTaskMenuFactory
VideoPlayerTaskMenuFactory::VideoPlayerTaskMenuFactory(QExtensionManager *parent) :
    QExtensionFactory(parent)
{
}

QObject *VideoPlayerTaskMenuFactory::createExtension(QObject *object,
                         const QString &iid, QObject *parent) const
{
    if (iid != Q_TYPEID(QDesignerTaskMenuExtension))
        return 0;

    if (Phonon::VideoPlayer *widget = qobject_cast<Phonon::VideoPlayer *>(object))
        return new VideoPlayerTaskMenu(widget, parent);

    return 0;
}
