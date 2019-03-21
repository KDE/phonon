/*
    Copyright (C) 2006-2007 Matthias Kretz <kretz@kde.org>
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

#ifndef PHONON_OBJECTDESCRIPTION_H
#define PHONON_OBJECTDESCRIPTION_H

#include "phonon_export.h"

#include <QtCore/QExplicitlySharedDataPointer>
#include <QtCore/QtDebug>
#include <QtCore/QList>
#include <QtCore/QSharedData>
#include <QtCore/QString>
#include <QtCore/QVariant>


namespace Phonon
{
    class ObjectDescriptionPrivate;

    /**
     * Defines the type of information that is contained in a ObjectDescription
     * object.
     *
     * \ingroup Backend
     */
    enum ObjectDescriptionType
    {
        /**
         * Audio output devices. This can be soundcards (with different drivers), soundservers or
         * other virtual outputs like playback on a different computer on the
         * network.
         *
         * For Hardware devices the backend should use libkaudiodevicelist
         * (AudioDevice and AudioDeviceEnumerator) which will list removable
         * devices even when they are unplugged and provide a unique identifier
         * that can make backends use the same identifiers.
         */
        AudioOutputDeviceType,

        /**
         * Lists all processing effects the backend supports.
         */
        EffectType,
        AudioChannelType,
        SubtitleType,

        /**
         * Audio capture devices. This can be soundcards (with different drivers), soundservers or
         * other virtual inputs like capture on a different computer on the
         * network.
         *
         * For Hardware devices the backend should use libkaudiodevicelist
         * (AudioDevice and AudioDeviceEnumerator) which will list removable
         * devices even when they are unplugged and provide a unique identifier
         * that can make backends use the same identifiers.
         */
        AudioCaptureDeviceType,

        /**
         * Video capture devices. Includes webcams.
         */
        VideoCaptureDeviceType

        //VideoOutputDeviceType,
        //AudioCodecType,
        //VideoCodecType,
        //ContainerFormatType,
        //VisualizationType,
    };

/** \internal
 * \class ObjectDescriptionData objectdescription.h phonon/ObjectDescription
 * \brief Data class for objects describing devices or features of the backend.
 *
 * \author Matthias Kretz <kretz@kde.org>
 * \see BackendCapabilities
 */
class PHONON_EXPORT ObjectDescriptionData : public QSharedData //krazy:exclude=dpointer (it's protected, which should be fine for this type of class)
{
    public:
        /**
         * Returns \c true if this ObjectDescription describes the same
         * as \p otherDescription; otherwise returns \c false.
         */
        bool operator==(const ObjectDescriptionData &otherDescription) const;

        /**
         * Returns the name of the capture source.
         *
         * \return A string that should be presented to the user to
         * choose the capture source.
         */
        QString name() const;

        /**
         * Returns a description of the capture source. This text should
         * make clear what sound source this is, which is sometimes hard
         * to describe or understand from just the name.
         *
         * \return A string describing the capture source.
         */
        QString description() const;

        /**
         * Returns a named property.
         *
         * If the property is not set an invalid value is returned.
         *
         * \see propertyNames()
         */
        QVariant property(const char *name) const;

        /**
         * Returns all names that return valid data when property() is called.
         *
         * \see property()
         */
        QList<QByteArray> propertyNames() const;

        /**
         * Returns \c true if the Tuple is valid (index != -1); otherwise returns
         * \c false.
         */
        bool isValid() const;

        /**
         * A unique identifier for this device/. Used internally
         * to distinguish between the devices/.
         *
         * \return An integer that uniquely identifies every device/
         */
        int index() const;

        static ObjectDescriptionData *fromIndex(ObjectDescriptionType type, int index);

        ~ObjectDescriptionData();

        ObjectDescriptionData(ObjectDescriptionPrivate * = 0);
        ObjectDescriptionData(int index, const QHash<QByteArray, QVariant> &properties);

    protected:
        ObjectDescriptionPrivate *const d;

    private:
        ObjectDescriptionData &operator=(const ObjectDescriptionData &rhs);
};

template<ObjectDescriptionType T> class ObjectDescriptionModel;

/** \class ObjectDescription objectdescription.h phonon/ObjectDescription
 * \short Provides a tuple of enduser visible name and description.
 *
 * Some parts give the enduser choices, e.g. what source to capture audio from.
 * These choices are described by the name and description methods of this class
 * and identified with the id method. Subclasses then define additional
 * information like which audio and video choices belong together.
 *
 * \ingroup Frontend
 * \author Matthias Kretz <kretz@kde.org>
 */
template<ObjectDescriptionType T>
class ObjectDescription
{
    public:
        /**
         * Returns a new description object that describes the
         * device/effect/codec/...  with the given \p index.
         */
        static inline ObjectDescription<T> fromIndex(int index) { //krazy:exclude=inline
            return ObjectDescription<T>(QExplicitlySharedDataPointer<ObjectDescriptionData>(ObjectDescriptionData::fromIndex(T, index)));
        }

        /**
         * Returns \c true if this ObjectDescription describes the same
         * as \p otherDescription; otherwise returns \c false.
         */
        inline bool operator==(const ObjectDescription &otherDescription) const { //krazy:exclude=inline
            return *d == *otherDescription.d;
        }

        /**
         * Returns \c false if this ObjectDescription describes the same
         * as \p otherDescription; otherwise returns \c true.
         */
        inline bool operator!=(const ObjectDescription &otherDescription) const { //krazy:exclude=inline
            return !operator==(otherDescription);
        }

        /**
         * Returns the name of the capture source.
         *
         * \return A string that should be presented to the user to
         * choose the capture source.
         */
        inline QString name() const { return d->name(); } //krazy:exclude=inline

        /**
         * Returns a description of the capture source. This text should
         * make clear what sound source this is, which is sometimes hard
         * to describe or understand from just the name.
         *
         * \return A string describing the capture source.
         */
        inline QString description() const { return d->description(); } //krazy:exclude=inline

        /**
         * Returns a named property.
         *
         * If the property is not set an invalid value is returned.
         *
         * \see propertyNames()
         */
        inline QVariant property(const char *name) const { return d->property(name); } //krazy:exclude=inline

        /**
         * Returns all names that return valid data when property() is called.
         *
         * \see property()
         */
        inline QList<QByteArray> propertyNames() const { return d->propertyNames(); } //krazy:exclude=inline

        /**
         * Returns \c true if the Tuple is valid (index != -1); otherwise returns
         * \c false.
         */
        inline bool isValid() const { return d->isValid(); } //krazy:exclude=inline

        /**
         * A unique identifier for this device/. Used internally
         * to distinguish between the devices/.
         *
         * \return An integer that uniquely identifies every device/
         */
        inline int index() const { return d->index(); } //krazy:exclude=inline

        ObjectDescription() : d(new ObjectDescriptionData(0)) {}
        ObjectDescription(int index, const QHash<QByteArray, QVariant> &properties) : d(new ObjectDescriptionData(index, properties)) {}

    protected:
        friend class ObjectDescriptionModel<T>;
        ObjectDescription(const QExplicitlySharedDataPointer<ObjectDescriptionData> &dd) : d(dd) {}
        QExplicitlySharedDataPointer<ObjectDescriptionData> d;
};

template<ObjectDescriptionType T>
QDebug operator<<(QDebug dbg, const ObjectDescription<T> &d)
{
    dbg.nospace() << "\n{\n";
    dbg.nospace() << "  index: " << d.index() << "\n";
    Q_FOREACH (const QByteArray &propertyName, d.propertyNames()) {
        dbg.nospace() << "  " << propertyName << ": " <<
                         d.property(propertyName).toString() << "\n";
    }
    dbg.nospace() << "}\n";

    return dbg.space();
}

/**
 * \ingroup BackendInformation
 */
typedef ObjectDescription<AudioOutputDeviceType> AudioOutputDevice;
/**
 * \ingroup BackendInformation
 */
#ifndef PHONON_NO_AUDIOCAPTURE
typedef ObjectDescription<AudioCaptureDeviceType> AudioCaptureDevice;
#endif //PHONON_NO_AUDIOCAPTURE
/**
 * \ingroup BackendInformation
 */
//typedef ObjectDescription<VideoOutputDeviceType> VideoOutputDevice;
/**
 * \ingroup BackendInformation
 */
#ifndef PHONON_NO_VIDEOCAPTURE
typedef ObjectDescription<VideoCaptureDeviceType> VideoCaptureDevice;
#endif
/**
 * \ingroup BackendInformation
 */
#ifndef QT_NO_PHONON_EFFECT
typedef ObjectDescription<EffectType> EffectDescription;
#endif //QT_NO_PHONON_EFFECT

/**
 * \ingroup BackendInformation
 */
//typedef ObjectDescription<AudioCodecType> AudioCodecDescription;
/**
 * \ingroup BackendInformation
 */
//typedef ObjectDescription<VideoCodecType> VideoCodecDescription;
/**
 * \ingroup BackendInformation
 */
//typedef ObjectDescription<ContainerFormatType> ContainerFormatDescription;
/**
 * \ingroup BackendInformation
 */
//typedef ObjectDescription<VisualizationType> VisualizationDescription;
#ifndef QT_NO_PHONON_MEDIACONTROLLER
typedef ObjectDescription<AudioChannelType> AudioChannelDescription;
typedef ObjectDescription<SubtitleType> SubtitleDescription;
#endif //QT_NO_PHONON_MEDIACONTROLLER

/**
 * \short Information about how to access a device
 * \ingroup BackendInformation
 *
 * To access a device, one needs the driver name (alsa, oss, pulse for example),
 * and the device name (dependent on the driver name). This type is a pair of a
 * driver and a device name.
 *
 * \see DeviceAccessList
 */
typedef QPair<QByteArray, QString> DeviceAccess;

/**
 * \short Information about methods for accessing a device
 * \ingroup BackendInformation
 *
 * It is used by the platform plugin or the backend to provide information about how
 * to access a certain device. To access a device, one needs the driver name (alsa, oss,
 * pulse for example), and the device name (dependent on the driver name). This type
 * is essentialy a list of pairs of driver and device names.
 *
 * It can be put in an ObjectDescriptionData property list.
 *
 * \see DeviceAccess
 * \see AudioCaptureDevice
 */
typedef QList<DeviceAccess> DeviceAccessList;

void PHONON_EXPORT_DEPRECATED registerMetaTypes();

} //namespace Phonon

Q_DECLARE_METATYPE(Phonon::AudioOutputDevice)
Q_DECLARE_METATYPE(QList<Phonon::AudioOutputDevice>)

#ifndef PHONON_NO_AUDIOCAPTURE
Q_DECLARE_METATYPE(Phonon::AudioCaptureDevice)
Q_DECLARE_METATYPE(QList<Phonon::AudioCaptureDevice>)
#endif //PHONON_NO_AUDIOCAPTURE

#ifndef PHONON_NO_VIDEOCAPTURE
Q_DECLARE_METATYPE(Phonon::VideoCaptureDevice)
Q_DECLARE_METATYPE(QList<Phonon::VideoCaptureDevice>)
#endif //PHONON_NO_VIDEOCAPTURE

#ifndef QT_NO_PHONON_EFFECT
Q_DECLARE_METATYPE(QList<Phonon::EffectDescription>)
Q_DECLARE_METATYPE(Phonon::EffectDescription)
#endif //QT_NO_PHONON_EFFECT


#ifndef QT_NO_PHONON_MEDIACONTROLLER
Q_DECLARE_METATYPE(Phonon::AudioChannelDescription)
Q_DECLARE_METATYPE(Phonon::SubtitleDescription)
Q_DECLARE_METATYPE(QList<Phonon::AudioChannelDescription>)
Q_DECLARE_METATYPE(QList<Phonon::SubtitleDescription>)
#endif //QT_NO_PHONON_MEDIACONTROLLER

Q_DECLARE_METATYPE(Phonon::DeviceAccess)
Q_DECLARE_METATYPE(Phonon::DeviceAccessList)



#endif // PHONON_OBJECTDESCRIPTION_H
