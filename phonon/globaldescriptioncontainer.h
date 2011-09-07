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

#ifndef PHONON_GLOBALDESCRIPTIONCONTAINER_H
#define PHONON_GLOBALDESCRIPTIONCONTAINER_H

#include <QtCore/QMap>
#include <QtCore/QDebug>
#include <QtCore/QtGlobal>

#include <phonon/objectdescription.h>

namespace Phonon
{

class MediaController;

/**
 * \internal
 *
 * A container for object descriptions.
 * The primary use of this container is to turn ObjectDescriptions with unique
 * id scope smaller than backend into globally unique ones.
 *
 * For example a MediaController enhances a MediaObject, you may have multiple
 * MediaObjects and thus MediaControllers in one application. The interface
 * to query all subtitles is only available on the backend class itself though,
 * so the index/id of the descriptions handed to a Phonon API user must be
 * unique with global scope.
 * This is where the GlobalDescriptionContainer comes in. It allows arbitrary
 * objects to register (using object address) with the container (which is a
 * singleton).
 * The object hands its locally unique ObjectDescriptions to the container, which
 * turns it into a globally unqiue description and maps the global id to the
 * local id.
 *
 * That way it is possible to easily map local to global description objects.
 *
 * \author Harald Sitter <sitter@kde.org>
 */
template <typename D>
class GlobalDescriptionContainer
{
public:
    typedef int global_id_t;
    typedef int local_id_t;

    typedef QMap<global_id_t, D> GlobalDescriptorMap;
    typedef QMapIterator<global_id_t, D> GlobalDescriptorMapIterator;

    typedef QMap<global_id_t, local_id_t> LocalIdMap;
    typedef QMapIterator<global_id_t, local_id_t> LocaIdMapIterator;

public:
    static GlobalDescriptionContainer *self;

    static GlobalDescriptionContainer *instance()
    {
        if (!self)
            self = new GlobalDescriptionContainer;
        return self;
    }

    virtual ~GlobalDescriptionContainer() {}

    /**
     * \returns a list of all global unique IDs of all stored ObjectDescriptions
     */
    QList<int> globalIndexes()
    {
        QList<int> list;
        GlobalDescriptorMapIterator it(m_globalDescriptors);
        while (it.hasNext()) {
            it.next();
            list << it.key();
        }
        return list;
    }

    /**
     * \param key the global ID of the ObjectDescription
     *
     * \returns ObjectDescriptions associated with a given ID
     */
    D fromIndex(global_id_t key)
    {
        return m_globalDescriptors.value(key, D());
    }

    // ----------- MediaController Specific ----------- //

    /**
     * Registers a new object within the container.
     * This essentially creates a new empty ID map.
     *
     * \param obj The reference object
     */
    void register_(void *obj)
    {
        Q_ASSERT(obj);
        Q_ASSERT(m_localIds.find(obj) == m_localIds.end());
        m_localIds[obj] = LocalIdMap();
    }

    /**
     * Unregisters a object from the container.
     * This essentially clears the ID map and removes all traces of the
     * object.
     *
     * \param obj The object
     */
    void unregister_(void *obj)
    {
        // TODO: remove all descriptions that are *only* associated with this MC
        Q_ASSERT(obj);
        Q_ASSERT(m_localIds.find(obj) != m_localIds.end());
        m_localIds[obj].clear();
        m_localIds.remove(obj);
    }

    /**
     * Clear the internal mapping of global to local id for a given object.
     *
     * \param obj The object
     */
    void clearListFor(void *obj)
    {
        Q_ASSERT(obj);
        Q_ASSERT_X(m_localIds.find(obj) != m_localIds.end(),
                   "clearing list",
                   "the object is not registered!");
        m_localIds[obj].clear();
    }

    /**
     * Adds a new description object for a specific object.
     * A description object *must* have a global unique id, which is ensured
     * by using this function, which will either reuse an existing equal
     * ObjectDescription or use the next free unique ID.
     * Using the provided index the unique ID is then mapped to the one of the
     * specific object.
     *
     * \param obj The object
     * \param index local ID (i.e. within the object @obj)
     * \param name Name of the description
     * \param type Type of the description (e.g. file)
     */
    void add(void *obj,
             local_id_t index, const QString &name, const QString &type = QString())
    {
        Q_ASSERT(obj);
        Q_ASSERT(m_localIds.find(obj) != m_localIds.end());

        QHash<QByteArray, QVariant> properties;
        properties.insert("name", name);
        properties.insert("description", "");
        properties.insert("type", type);

        // Empty lists will start at 0.
        global_id_t id = 0;
        {
            // Find id, either a descriptor with name and type is already present
            // or get the next available index.
            GlobalDescriptorMapIterator it(m_globalDescriptors);
            while (it.hasNext()) {
                it.next();
                if (it.value().property("name") == name &&
                        it.value().property("type") == type) {
                    id = it.value().index();
                }
            }
            if (id == 0)
                id = nextFreeIndex();
        }
        D descriptor = D(id, properties);

        m_globalDescriptors.insert(id, descriptor);
        m_localIds[obj].insert(id, index);
    }

    /**
     * Overload function.
     * The index of the provided descriptor *must* be unique within the
     * context of the container.
     *
     * \param obj The object
     * \param descriptor the DescriptionObject with unique index
     */
    void add(void *obj,
             D descriptor)
    {
        Q_ASSERT(obj);
        Q_ASSERT(m_localIds.find(obj) != m_localIds.end());
        Q_ASSERT(m_globalDescriptors.find(descriptor.index()) == m_globalDescriptors.end());

        m_globalDescriptors.insert(descriptor.index(), descriptor);
        m_localIds[obj].insert(descriptor.index(), descriptor.index());
    }

    /**
     * List of ObjectDescriptions for a given object, the
     * descriptions are limied by the scope of the type (obviously), so you only
     * get ObjectDescription from the container.
     *
     * \param obj The object
     *
     * \returns the list of ObjectDescriptions for a given object, the
     * descriptions are limied by the scope of the type (obviously), so you only
     * get subtitle descriptions from a subtitle container.
     */
    QList<D> listFor(const void *obj) const
    {
        Q_ASSERT(obj);
        Q_ASSERT(m_localIds.find(obj) != m_localIds.end());

        QList<D> list;
        LocaIdMapIterator it(m_localIds.value(obj));
        while (it.hasNext()) {
            it.next();
            Q_ASSERT(m_globalDescriptors.find(it.key()) != m_globalDescriptors.end());
            list << m_globalDescriptors[it.key()];
        }
        return list;
    }

    /**
     * \param obj The object
     * \param key the global ID (i.e. index of an ObjectDescription)
     *
     * \returns the local ID associated with the description object
     */
    int localIdFor(const void *obj, global_id_t key) const
    {
        Q_ASSERT(obj);
        Q_ASSERT(m_localIds.find(obj) != m_localIds.end());
        if (m_localIds[obj].find(key) == m_localIds[obj].end())
            qWarning() << "WARNING:" << Q_FUNC_INFO
                       << ": supplied global ID is unknown for the object ("
                       << obj << ")";
        return m_localIds[obj].value(key, 0);
    }

protected:
    GlobalDescriptionContainer() :
        m_peak(0)
    {
    }

    /**
     * \returns next free unique index to be used as global ID for an ObjectDescription
     */
    global_id_t nextFreeIndex()
    {
        return ++m_peak;
    }

    GlobalDescriptorMap m_globalDescriptors;
    QMap<const void *, LocalIdMap> m_localIds;

    global_id_t m_peak;
};

template <typename D>
GlobalDescriptionContainer<D> *GlobalDescriptionContainer<D>::self = 0;

typedef GlobalDescriptionContainer<AudioChannelDescription> GlobalAudioChannels;
typedef GlobalDescriptionContainer<SubtitleDescription> GlobalSubtitles;

} // Namespace Phonon

#endif // PHONON_GLOBALDESCRIPTIONCONTAINER_H
