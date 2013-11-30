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

#ifndef PHONON_SOURCE_CONTROL_INTERFACE_H_
#define PHONON_SOURCE_CONTROL_INTERFACE_H_

#include "source.h"

#include "sourcecontrol.h"

#include <QtCore/QSet>

namespace Phonon
{

class SourceControl;

class SourceControlInterface
{
public:
    virtual ~SourceControlInterface() {}

    virtual bool isActive() const = 0;
};

class VideoSourceControlInterface : public SourceControlInterface
{
public:
    virtual bool supportsMenus() const = 0;
    virtual QSet<VideoSourceControl::Menu> availableMenus() const = 0;
    virtual VideoSourceControl::Menu currentMenu() const = 0;
    virtual void setCurrentMenu(VideoSourceControl::Menu menu) = 0;

    virtual bool supportsChapters() const = 0;
    virtual int chapterCount() const = 0;
    virtual int currentChapter() const = 0;
    virtual void setCurrentChapter(int chapterNumber) = 0;

    virtual bool supportsAngles() const = 0;
    virtual int angleCount() const = 0;
    virtual int currentAngle() = 0;
    virtual void setCurrentAngle(int angleNumber) = 0;

    virtual bool supportsTitles() const = 0;
    virtual bool isAutoplayingTitles() const = 0;
    virtual void setAutoplayTitles(bool enable) = 0;
    virtual int titleCount() const = 0;
    virtual int currentTitle() const = 0;
    virtual void setCurrentTitle(int titleNumber) = 0;

    virtual bool supportsAudioChannels() const = 0;
    virtual int audioChannelCount() const = 0;
    virtual int audioChannel() const = 0;
    virtual void setAudioChannel(int channelNumber) = 0;

Q_SIGNALS:
    virtual void availableMenusChanged() = 0;
    virtual void currentMenuChanged(VideoSourceControl::Menu menu) = 0;

    virtual void chapterCountChanged(int count) = 0;
    virtual void currentChapterChanged(int chapterNumber) = 0;

    virtual void angleCountChanged(int count) = 0;
    virtual void currentAngleChanged(int angleNumber) = 0;

    virtual void titleCountChanged(int count) = 0;
    virtual void currentTitleChanged(int titleNumber) = 0;

    virtual void audioChannelCountChanged(int count) = 0;
};

class AudioSourceControlInterface : public SourceControlInterface
{
public:
    virtual bool supportsTracks() const = 0;
    virtual int trackCount() const = 0;
    virtual int trackNumber() const = 0;
    virtual void setTrackNumber(int trackNumber) = 0;

Q_SIGNALS:
    virtual void trackCountChanged(int count) = 0;
    virtual void currentTrackChanged(int trackNumber) = 0;
};

} // namespace Phonon

Q_DECLARE_INTERFACE(Phonon::VideoSourceControlInterface, "0VideoSourceControlInterface.phonon.kde.org")
Q_DECLARE_INTERFACE(Phonon::AudioSourceControlInterface, "0AudioSourceControlInterface.phonon.kde.org")

#endif // PHONON_SOURCE_CONTROL_INTERFACE_H_
