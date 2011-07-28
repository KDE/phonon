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

#ifndef ABSTRACTMEDIAELEMENT_H
#define ABSTRACTMEDIAELEMENT_H

#include <QtCore/QUrl>
#include <QtDeclarative/QDeclarativeItem>

#include <phonon/mediaobject.h>
#include <phonon/phononnamespace.h>

#include "abstractinitable.h"

namespace Phonon {

class MediaObject;

namespace Declarative {

/**
 * Macro to help with creating a property getter using the Phonon::MetaData enum.
 */
#define P_PROPERTY_GETTER(__cName) \
    public: QStringList __cName() const { return m_mediaObject->metaData(Phonon::__cName##MetaData ); } private:

class MetaData : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList artist READ Artist NOTIFY metaDataChanged)
    Q_PROPERTY(QStringList album READ Album NOTIFY metaDataChanged)
    Q_PROPERTY(QStringList title READ Title NOTIFY metaDataChanged)
    Q_PROPERTY(QStringList date READ Date NOTIFY metaDataChanged)
    Q_PROPERTY(QStringList genre READ Genre NOTIFY metaDataChanged)
    Q_PROPERTY(QStringList trackNumber READ Tracknumber NOTIFY metaDataChanged)
    Q_PROPERTY(QStringList description READ Description NOTIFY metaDataChanged)
    Q_PROPERTY(QStringList musicBrainzDiscId READ MusicBrainzDiscId NOTIFY metaDataChanged)
    P_PROPERTY_GETTER(Artist)
    P_PROPERTY_GETTER(Album)
    P_PROPERTY_GETTER(Title)
    P_PROPERTY_GETTER(Date)
    P_PROPERTY_GETTER(Genre)
    P_PROPERTY_GETTER(Tracknumber)
    P_PROPERTY_GETTER(Description)
    P_PROPERTY_GETTER(MusicBrainzDiscId)
public:
    MetaData(MediaObject *mediaObject, QObject *parent = 0) :
      QObject(parent),
      m_mediaObject(mediaObject)
    {
        connect(m_mediaObject, SIGNAL(metaDataChanged()),
                this, SIGNAL(metaDataChanged()));
    }
    ~MetaData() {}

signals:
    void metaDataChanged();

private:
    MediaObject *m_mediaObject;
};

#undef P_PROPERTY_GETTER

class MediaElement : public QDeclarativeItem, AbstractInitAble
{
    Q_OBJECT
    Q_INTERFACES(QDeclarativeParserStatus)
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)

    Q_PROPERTY(bool playing READ isPlaying NOTIFY playingChanged)
    Q_PROPERTY(bool paused READ isPaused NOTIFY pausedChanged)
    Q_PROPERTY(bool stopped READ isStopped NOTIFY stoppedChanged)

    Q_PROPERTY(bool video READ hasVideo NOTIFY hasVideoChanged)
    Q_PROPERTY(bool seekable READ isSeekable NOTIFY seekableChanged)
    Q_PROPERTY(qint32 tickInterval READ tickInterval WRITE setTickInterval)
    Q_PROPERTY(qreal totalTime READ totalTime NOTIFY totalTimeChanged)
    Q_PROPERTY(qreal time READ time WRITE seek NOTIFY timeChanged)

    Q_PROPERTY(Phonon::Declarative::MetaData *metaData READ metaData CONSTANT)
public:
    MediaElement(QDeclarativeItem *parent = 0);
    virtual ~MediaElement();

    void classBegin();
    void componentComplete();

    virtual void init(MediaObject *mediaObject = 0);

    MediaObject *mediaObject() const { return m_mediaObject; }

    QUrl source() const;
    void setSource(const QUrl &url);

    bool isPlaying() const;
    bool isPaused() const;
    bool isStopped() const;

    bool hasVideo() const;
    bool isSeekable() const;
    qint32 tickInterval() const;
    void setTickInterval(qint32 interval);
    qreal totalTime() const;
    qreal time() const;
    void seek(qreal time);

    MetaData *metaData();

signals:
    void sourceChanged();

    void stateChanged();
    void playingChanged();
    void pausedChanged();
    void stoppedChanged();

    void hasVideoChanged();
    void seekableChanged();
    void totalTimeChanged();
    void timeChanged();

public slots:
    void play();
    void pause();
    void stop();

private slots:
    void handleFinished();
    void handleStateChange(Phonon::State newState, Phonon::State oldState);

protected:
    State m_state;

    bool m_finished;

private:
    void emitStateChanges(Phonon::State state);

    QUrl m_source;

    MetaData *m_metaData;
};

} // namespace Declarative
} // namespace Phonon

QML_DECLARE_TYPE(Phonon::Declarative::MetaData)

#endif // ABSTRACTMEDIAELEMENT_H
