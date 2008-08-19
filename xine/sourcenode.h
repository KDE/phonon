/*  This file is part of the KDE project
    Copyright (C) 2007 Matthias Kretz <kretz@kde.org>

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

#ifndef SOURCENODE_H
#define SOURCENODE_H

#include <Phonon/Global>
#include <QtCore/QExplicitlySharedDataPointer>
#include <QtCore/QSet>
#include <xine.h>
#include "backend.h"
#include "shareddata.h"

namespace Phonon
{
namespace Xine
{
class SinkNode;
class Event;
class SinkNodeXT;

class SourceNodeXT : virtual public SharedData
{
    public:
        SourceNodeXT(const char *name = "SourceNode");
        virtual ~SourceNodeXT();
        virtual xine_post_out_t *audioOutputPort() const;
        virtual xine_post_out_t *videoOutputPort() const;
        void assert() { Q_ASSERT(!deleted); }

        const char *const className;

    private:
        bool deleted;

        friend class XineThread;

        // hold a ref to the sink that we're connected to to make 100% sure it's always valid
        QExplicitlySharedDataPointer<SinkNodeXT> m_xtSink;
};

class SourceNode
{
    friend class WireCall;
    public:
        SourceNode(SourceNodeXT *_xt);
        virtual ~SourceNode();
        virtual MediaStreamTypes outputMediaStreamTypes() const = 0;
        void addSink(SinkNode *s);
        void removeSink(SinkNode *s);
        QSet<SinkNode *> sinks() const;
        virtual SinkNode *sinkInterface();

        virtual void upstreamEvent(Event *);
        virtual void downstreamEvent(Event *);

        inline QExplicitlySharedDataPointer<SourceNodeXT> threadSafeObject() const { return m_threadSafeObject; }

    protected:
        QExplicitlySharedDataPointer<SourceNodeXT> m_threadSafeObject;
    private:
        QSet<SinkNode *> m_sinks;
};
} // namespace Xine
} // namespace Phonon

Q_DECLARE_INTERFACE(Phonon::Xine::SourceNode, "XineSourceNode.phonon.kde.org")

inline QDebug operator<<(QDebug &s, const Phonon::Xine::SourceNodeXT *const node)
{
    if (node->className) {
        s.nospace() << node->className << '(' << static_cast<const void *>(node) << ')';
    } else {
        s.nospace() << "no classname: " << static_cast<const void *>(node);
    }
    return s.space();
}

#endif // SOURCENODE_H
