/*  This file is part of the KDE project
    Copyright (C) 2007 Matthias Kretz <kretz@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), Trolltech ASA 
    (or its successors, if any) and the KDE Free Qt Foundation, which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public 
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef MEDIAOBJECTITEM_H
#define MEDIAOBJECTITEM_H

#include "widgetrectitem.h"
#include "titlewidget.h"
#include "chapterwidget.h"
#include "anglewidget.h"
#include "navigationwidget.h"
#include <Phonon/Global>
#include <Phonon/MediaObject>
#include <Phonon/SeekSlider>
#include <QtCore/QPointer>
#include <QtGui/QLabel>
#include <QtGui/QProgressBar>
#include <QtGui/QToolButton>
class QPushButton;

using Phonon::MediaNode;
using Phonon::SeekSlider;
using Phonon::MediaObject;

class MediaObjectItem : public WidgetRectItem
{
    Q_OBJECT
    public:
        MediaObjectItem(const QPoint &pos, QGraphicsView *widget);

        enum { Type = UserType + 2 };
        int type() const { return Type; }

        MediaNode *mediaNode() { return &m_media; }
        const MediaNode *mediaNode() const { return &m_media; }

    private slots:
        void loadUrl(const QString &);
        void updateMetaData();
        void openCD();
        void openDVD();
        void showTitleWidget(bool);
        void showChapterWidget(bool);
        void showAngleWidget(bool);
        void showNavigationWidget(bool);
        void stateChanged(Phonon::State newstate, Phonon::State oldstate);
        void length(qint64 ms);
        void slotFinished();
        void slotPrefinishMarkReached(qint32 remaining);
        void tick(qint64 t);

    private:
        SeekSlider *m_seekslider;
        QToolButton *m_play;
        QToolButton *m_pause;
        QToolButton *m_stop;
        QPushButton *m_titleButton;
        QPushButton *m_chapterButton;
        QPushButton *m_angleButton;
        QPushButton *m_navigationButton;
        QLabel *m_statelabel;
        QProgressBar *m_bufferProgress;
        QLabel *m_totaltime;
        QLabel *m_currenttime;
        QLabel *m_remainingtime;
        QLabel *m_metaDataLabel;
        MediaObject m_media;
        qint64 m_length;
        TitleWidget *m_titleWidget;
        ChapterWidget *m_chapterWidget;
        AngleWidget *m_angleWidget;
        NavigationWidget *m_navigationWidget;
        QPointer<MediaController> m_mediaController;
};

#endif // MEDIAOBJECTITEM_H
