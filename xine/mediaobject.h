/*  This file is part of the KDE project
    Copyright (C) 2006 Tim Beaulen <tbscope@gmail.com>
    Copyright (C) 2006-2007 Matthias Kretz <kretz@kde.org>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.

*/
#ifndef Phonon_XINE_MEDIAOBJECT_H
#define Phonon_XINE_MEDIAOBJECT_H

#include <QObject>
#include <phonon/mediaobjectinterface.h>
#include <phonon/addoninterface.h>

#include "xineengine.h"
#include "xinestream.h"

#include <QByteArray>
#include <QList>
#include <QTime>
#include <QHash>
#include <QMultiMap>
#include <QPointer>

#include <kurl.h>

#include <xine.h>
#include "sourcenode.h"

namespace Phonon
{
namespace Xine
{
class XineStream;
class ByteStream;

class MediaObject : public QObject, public MediaObjectInterface, public AddonInterface, public SourceNode
{
    Q_OBJECT
    Q_INTERFACES(Phonon::MediaObjectInterface Phonon::AddonInterface Phonon::Xine::SourceNode)
    public:
        MediaObject(QObject *parent);
        ~MediaObject();

        State state() const;
        bool hasVideo() const;
        bool isSeekable() const;
        qint64 currentTime() const;
        qint64 totalTime() const;
        Q_INVOKABLE qint64 remainingTime() const;
        qint32 tickInterval() const;

        void setTickInterval(qint32 newTickInterval);
        void play();
        void pause();
        void stop();
        void seek(qint64 time);

        QString errorString() const;
        Phonon::ErrorType errorType() const;

        QExplicitlySharedDataPointer<XineStream> stream() { return QExplicitlySharedDataPointer<XineStream>(m_stream); }
        const QExplicitlySharedDataPointer<XineStream> stream() const { return QExplicitlySharedDataPointer<XineStream>(m_stream); }

        bool hasInterface(AddonInterface::Interface i) const;
        QVariant interfaceCall(AddonInterface::Interface, int, const QList<QVariant> &);

        Q_INVOKABLE qint32 prefinishMark() const;
        Q_INVOKABLE void setPrefinishMark(qint32 newPrefinishMark);

        Q_INVOKABLE qint32 transitionTime() const;
        Q_INVOKABLE void setTransitionTime(qint32 newTransitionTime);

        MediaSource source() const;
        void setSource(const MediaSource &source);
        void setNextSource(const MediaSource &source);

        MediaStreamTypes outputMediaStreamTypes() const;
        void upstreamEvent(Event *e);

    public slots:
        void downstreamEvent(Event *e);

    signals:
        void aboutToFinish();
        void finished();
        void prefinishMarkReached(qint32 msec);
        void totalTimeChanged(qint64 length);
        void currentSourceChanged(const MediaSource &);

        void stateChanged(Phonon::State newstate, Phonon::State oldstate);
        void tick(qint64 time);
        void metaDataChanged(const QMultiMap<QString, QString> &);
        void seekableChanged(bool);
        void hasVideoChanged(bool);
        void bufferStatus(int);
        void asyncSeek(xine_stream_t *, qint64, bool);

        // AddonInterface
        void availableSubtitlesChanged();
        void availableAudioChannelsChanged();
        void availableTitlesChanged(int);
        void titleChanged(int);
        void availableChaptersChanged(int);
        void chapterChanged(int);
        void availableAnglesChanged(int);
        void angleChanged(int);

    protected slots:
        void startToFakeBuffering();

    private slots:
        void handleStateChange(Phonon::State newstate, Phonon::State oldstate);
        void needNextUrl();
        void handleAvailableTitlesChanged(int);
        void handleFinished();
        void handleHasVideoChanged(bool);
        void syncHackSetProperty(const char *name, const QVariant &val);

    private:
        enum HowToSetTheUrl {
            GaplessSwitch,
            HardSwitch
        };
        void setSourceInternal(const MediaSource &, HowToSetTheUrl);
        QByteArray autoplayMrlsToTitles(const char *plugin, const char *defaultMrl);

        Phonon::State m_state;
        XineStream *m_stream;
        qint32 m_tickInterval;
        QPointer<ByteStream> m_bytestream;

        mutable int m_currentTimeOverride;
        MediaSource m_mediaSource;
        QList<QByteArray> m_titles;
        QByteArray m_mediaDevice;
        int m_currentTitle;
        qint32 m_prefinishMark;
        qint32 m_transitionTime;
        bool m_autoplayTitles : 1;
        bool m_fakingBuffering : 1;
        bool m_shouldFakeBufferingOnPlay : 1;
};
}} //namespace Phonon::Xine

// vim: sw=4 ts=4 tw=80
#endif // Phonon_XINE_MEDIAOBJECT_H
