/*
 * This file is part of the KDE project
 * Copyright (C) 2013 Casian Andrei <skeletk13@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) version 3, or any
 * later version accepted by the membership of KDE e.V. (or its
 * successor approved by the membership of KDE e.V.), Nokia Corporation
 * (or its successors, if any) and the KDE Free Qt Foundation, which shall
 * act as a proxy defined in Section 6 of version 3 of the license.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PHONON_SOURCE_CONTROL_H_
#define PHONON_SOURCE_CONTROL_H_

#include "frontend.h"
#include "source.h"

#include <QtCore/QObject>

namespace Phonon
{

class SourceControlPrivate;
class VideoSourceControlPrivate;
class AudioSourceControlPrivate;

class PHONON_EXPORT SourceControl : public QObject, public Frontend
{
    Q_OBJECT

public:
    SourceControl(SourceControlPrivate &pd, QObject *parent = 0);
    virtual ~SourceControl();

    Source source() const;
    bool isActive() const;

private:
    P_DECLARE_PRIVATE(SourceControl)
};

class PHONON_EXPORT VideoSourceControl : public SourceControl
{
    Q_OBJECT

public:
    enum Menu {
        RootMenu,
        TitleMenu,
        AudioMenu,
        SubtitleMenu,
        ChapterMenu,
        AngleMenu
    };

public:
    explicit VideoSourceControl(Source &source, QObject *parent = 0);
    virtual ~VideoSourceControl();

    bool supportsMenus() const;
    QSet<Menu> availableMenus() const;
    Menu currentMenu() const;
    void setCurrentMenu(Menu menu);

    bool supportsChapters() const;
    int chapterCount() const;
    int currentChapter() const;
    void setCurrentChapter(int chapterNumber);

    bool supportsAngles() const;
    int angleCount() const;
    int currentAngle();
    void setCurrentAngle(int angleNumber);

    bool supportsTitles() const;
    bool isAutoplayingTitles() const;
    void setAutoplayTitles(bool enable);
    int titleCount() const;
    int currentTitle() const;
    void setCurrentTitle(int titleNumber);

    bool supportsAudioChannels() const;
    int audioChannelCount() const;
    int audioChannel() const;
    void setAudioChannel(int channelNumber);

Q_SIGNALS:
    void availableMenusChanged();
    void currentMenuChanged(Menu menu);

    void chapterCountChanged(int count);
    void currentChapterChanged(int chapterNumber);

    void angleCountChanged(int count);
    void currentAngleChanged(int angleNumber);

    void titleCountChanged(int count);
    void currentTitleChanged(int titleNumber);

    void audioChannelCountChanged(int count);

private:
    P_DECLARE_PRIVATE(VideoSourceControl)
};

class PHONON_EXPORT AudioSourceControl : public SourceControl
{
    Q_OBJECT

public:
    explicit AudioSourceControl(Source &source, QObject *parent = 0);
    virtual ~AudioSourceControl();

    bool supportsTracks() const;
    int trackCount() const;
    int trackNumber() const;
    void setTrackNumber(int trackNumber);

Q_SIGNALS:
    void trackCountChanged(int count);
    void currentTrackChanged(int trackNumber);

private:
    P_DECLARE_PRIVATE(AudioSourceControl)
};

} // Phonon namespace

#endif // PHONON_SOURCE_CONTROL_H_
