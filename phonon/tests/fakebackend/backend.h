/*  This file is part of the KDE project
    Copyright (C) 2004-2006 Matthias Kretz <kretz@kde.org>

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

#ifndef Phonon_FAKE_BACKEND_H
#define Phonon_FAKE_BACKEND_H

#include <phonon/objectdescription.h>
#include <phonon/backendinterface.h>

#include <QtCore/QList>
#include <QtCore/QPointer>
#include <QtCore/QStringList>

class KUrl;

#ifdef MAKE_PHONON_FAKE_LIB /* We are building this library */
# define PHONON_FAKE_EXPORT Q_DECL_EXPORT
#else /* We are using this library */
# define PHONON_FAKE_EXPORT Q_DECL_IMPORT
#endif

namespace Phonon
{
namespace Fake
{
    class AudioOutput;

    class PHONON_FAKE_EXPORT Backend : public QObject, public BackendInterface
    {
        Q_OBJECT
        Q_INTERFACES(Phonon::BackendInterface)
        public:
            Backend(QObject *parent = 0, const QVariantList & = QVariantList());
            virtual ~Backend();

            QObject *createObject(BackendInterface::Class, QObject *parent, const QList<QVariant> &args);

            bool supportsVideo() const;
            bool supportsOSD() const;
            bool supportsFourcc(quint32 fourcc) const;
            bool supportsSubtitles() const;
            QStringList availableMimeTypes() const;

            void freeSoundcardDevices();

            QList<int> objectDescriptionIndexes(ObjectDescriptionType type) const;
            QHash<QByteArray, QVariant> objectDescriptionProperties(ObjectDescriptionType type, int index) const;

            bool startConnectionChange(QSet<QObject *>);
            bool connectNodes(QObject *, QObject *);
            bool disconnectNodes(QObject *, QObject *);
            bool endConnectionChange(QSet<QObject *>);

        Q_SIGNALS:
            void objectDescriptionChanged(ObjectDescriptionType);

        private:
            QStringList m_supportedMimeTypes;
            QList<QPointer<AudioOutput> > m_audioOutputs;
    };
}} // namespace Phonon::Fake

// vim: sw=4 ts=4 tw=80
#endif // Phonon_FAKE_BACKEND_H
