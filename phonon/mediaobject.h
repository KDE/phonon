/*  This file is part of the KDE project
    Copyright (C) 2005 Matthias Kretz <kretz@kde.org>

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
#ifndef Phonon_MEDIAOBJECT_H
#define Phonon_MEDIAOBJECT_H

#include "medianode.h"
#include "mediasource.h"
#include "phonon_export.h"
#include "phonondefs.h"
#include "phononnamespace.h"


namespace Phonon
{
    class MediaObjectPrivate;

    class PHONON_EXPORT MediaObject : public QObject, public MediaNode
    {
        Q_OBJECT

        Q_PROPERTY(qint32 tickInterval READ tickInterval WRITE setTickInterval)

        public:
            MediaObject(QObject *parent = 0);

            /**
             * Destroys the MediaObject.
             */
            ~MediaObject();

            State state() const;

            bool hasVideo() const;

            bool isSeekable() const;

            qint32 tickInterval() const;

            QStringList metaData(const QString &key) const;

            QStringList metaData(Phonon::MetaData key) const;

            QMultiMap<QString, QString> metaData() const;

            QString errorString() const;

            ErrorType errorType() const;

            MediaSource currentSource() const;

            void setCurrentSource(const MediaSource &source);

            qint64 currentTime() const;

            qint64 totalTime() const;

            qint64 remainingTime() const;

        public Q_SLOTS:
            void setTickInterval(qint32 newTickInterval);
            void play();
            void pause();
            void stop();

            void seek(qint64 time);

        Q_SIGNALS:
            /**
             * Emitted when the state of the MediaObject has changed.
             *
             * @param newstate The state the Player is in now.
             * @param oldstate The state the Player was in before.
             */
            void stateChanged(Phonon::State newstate, Phonon::State oldstate);

            /**
             * This signal gets emitted every tickInterval milliseconds.
             *
             * @param time The position of the media file in milliseconds.
             *
             * @see setTickInterval, tickInterval
             */
            void tick(qint64 time);

            /**
             * This signal is emitted whenever the audio/video data that is
             * being played is associated with new meta data. E.g. for radio
             * streams this happens when the next song is played.
             *
             * You can get the new meta data with the metaData methods.
             */
            void metaDataChanged();

            /**
             * Emitted whenever the return value of isSeekable() changes.
             *
             * Normally you'll check isSeekable() first and then let this signal
             * tell you whether seeking is possible now or not. That way you
             * don't have to poll isSeekable().
             *
             * \param isSeekable \p true  if the stream is seekable (i.e. calling
             *                            seek() works)
             *                   \p false if the stream is not seekable (i.e.
             *                            all calls to seek() will be ignored)
             */
            void seekableChanged(bool isSeekable);

#ifndef QT_NO_PHONON_VIDEO
            void hasVideoChanged(bool hasVideo);
#endif //QT_NO_PHONON_VIDEO

            void bufferStatus(int percentFilled);

            void finished();

            void currentSourceChanged(const Phonon::MediaSource &newSource);

            void totalTimeChanged(qint64 newTotalTime);

        private:
            P_DECLARE_PRIVATE(MediaObject)
            Q_PRIVATE_SLOT(k_func(), void _k_resumePlay())
            Q_PRIVATE_SLOT(k_func(), void _k_resumePause())
            Q_PRIVATE_SLOT(k_func(), void _k_metaDataChanged(const QMultiMap<QString, QString> &))
#ifndef QT_NO_PHONON_ABSTRACTMEDIASTREAM
            Q_PRIVATE_SLOT(k_func(), void _k_stateChanged(Phonon::State, Phonon::State))
#endif //QT_NO_PHONON_ABSTRACTMEDIASTREAM
            Q_PRIVATE_SLOT(k_func(), void _k_currentSourceChanged(const MediaSource &))
            Q_PRIVATE_SLOT(k_func(), void _k_stateChanged(Phonon::State, Phonon::State))
    };
} //namespace Phonon

#endif // Phonon_MEDIAOBJECT_H
