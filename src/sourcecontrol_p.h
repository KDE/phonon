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

#ifndef PHONON_SOURCE_CONTROL_P_H_
#define PHONON_SOURCE_CONTROL_P_H_

#include "sourcecontrol.h"
#include "sourcecontrolinterface.h"

#include "frontend_p.h"
#include "phononpimpl_p.h"

namespace Phonon
{

class SourceControlPrivate : public FrontendPrivate
{
public:
    explicit SourceControlPrivate(Source &source);
    virtual ~SourceControlPrivate();

    SourceControlInterface *scInterface;

    Source source;

private:
    P_DECLARE_PUBLIC(SourceControl)
    Q_DISABLE_COPY(SourceControlPrivate);
};

class VideoSourceControlPrivate : public SourceControlPrivate
{
public:
    explicit VideoSourceControlPrivate(Source &source);
    virtual ~VideoSourceControlPrivate();

    VideoSourceControlInterface *interface;

    VideoSourceControl::Menu currentMenu;
    int currentChapter;
    int currentAngle;
    int currentTitle;
    bool autoplayTitlesEnabled;
    int audioChannel;

protected:
    virtual void createBackendObject() Q_DECL_OVERRIDE Q_DECL_FINAL;
    virtual void setupBackendObject() Q_DECL_OVERRIDE Q_DECL_FINAL;

private:
    P_DECLARE_PUBLIC(VideoSourceControl)
    Q_DISABLE_COPY(VideoSourceControlPrivate);
};

class AudioSourceControlPrivate : public SourceControlPrivate
{
public:
    explicit AudioSourceControlPrivate(Source &source);
    virtual ~AudioSourceControlPrivate();

    AudioSourceControlInterface *interface;

    int currentTrack;

protected:
    virtual void createBackendObject() Q_DECL_OVERRIDE Q_DECL_FINAL;
    virtual void setupBackendObject() Q_DECL_OVERRIDE Q_DECL_FINAL;

private:
    P_DECLARE_PUBLIC(AudioSourceControl)
    Q_DISABLE_COPY(AudioSourceControlPrivate);
};

} // Phonon namespace

#endif // PHONON_SOURCE_CONTROL_P_H_
