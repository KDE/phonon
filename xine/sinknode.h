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

#ifndef SINKNODE_H
#define SINKNODE_H

#include <Phonon/Global>
#include <xine.h>
#include "backend.h"
#include "shareddata.h"

namespace Phonon
{
namespace Xine
{
class SourceNode;
class SourceNodeXT;
class Event;

class SinkNodeXT : virtual public SharedData
{
    public:
        SinkNodeXT(const char *name = "SinkNode") : className(name), deleted(false) {}
        virtual ~SinkNodeXT();
        virtual void rewireTo(SourceNodeXT *) = 0;
        virtual xine_audio_port_t *audioPort() const;
        virtual xine_video_port_t *videoPort() const;
        void assert() { Q_ASSERT(!deleted); }

        XineEngine m_xine;
        const char *const className;

    private:
        bool deleted;
};

class SinkNode
{
    friend class WireCall;
    friend class XineStream;
    public:
        SinkNode(SinkNodeXT *_xt);
        virtual ~SinkNode();
        virtual MediaStreamTypes inputMediaStreamTypes() const = 0;
        void setSource(SourceNode *s);
        void unsetSource(SourceNode *s);
        SourceNode *source() const;
        virtual SourceNode *sourceInterface();

        virtual void upstreamEvent(Event *);
        virtual void downstreamEvent(Event *);

        void findXineEngine();
        inline QExplicitlySharedDataPointer<SinkNodeXT> threadSafeObject() const { return m_threadSafeObject; }

    protected:
        virtual void xineEngineChanged() {}
        virtual void aboutToChangeXineEngine() {}

        QExplicitlySharedDataPointer<SinkNodeXT> m_threadSafeObject;

    private:
        SourceNode *m_source;
};

#define K_XT(Class) Class##XT *xt = static_cast<Class##XT *>(SinkNode::m_threadSafeObject.data())

} // namespace Xine
} // namespace Phonon

Q_DECLARE_INTERFACE(Phonon::Xine::SinkNode, "XineSinkNode.phonon.kde.org")

inline QDebug operator<<(QDebug &s, const Phonon::Xine::SinkNodeXT *const node)
{
    if (node->className) {
        s.nospace() << node->className << '(' << static_cast<const void *>(node) << ')';
    } else {
        s.nospace() << "no classname: " << static_cast<const void *>(node);
    }
    return s.space();
}

#endif // SINKNODE_H
