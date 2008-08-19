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


#ifndef WIRECALL_H
#define WIRECALL_H

#include "sinknode.h"
#include "sourcenode.h"

namespace Phonon
{
namespace Xine
{

class WireCall
{
    public:
        WireCall() : src(0), snk(0) {}
        WireCall(SourceNode *a, SinkNode *b) : source(a->threadSafeObject()), sink(b->threadSafeObject()), src(a), snk(b) {}
        WireCall(SourceNode *a, const QExplicitlySharedDataPointer<SinkNodeXT> &b) : source(a->threadSafeObject()), sink(b), src(a), snk(0) {}

        /**
         * If the two WireCalls are in separate graphs treat them as equal (returns false)
         *
         * If the two WireCalls have the same source treat them as equal (returns false)
         *
         * If the two WireCalls are in the same graph:
         * returns false if this wire is a source for \p rhs
         * returns true if \p rhs is a source for this wire
         */
        bool operator<(const WireCall &rhs) const {
            if (src == rhs.src) {
                // treat the wire calls as equal
                return false;
            }
            if (!snk || !rhs.snk) {
                return false;
            }
            SourceNode *s = src;
            while (s && s->sinkInterface()) {
                if (rhs.snk == s->sinkInterface()) {
                    return true;
                }
                s = s->sinkInterface()->source();
            }
            return false;
        }

        bool operator==(const WireCall &rhs) const
        {
            return source == rhs.source && sink == rhs.sink;
        }

        void addReferenceTo(const QList<QExplicitlySharedDataPointer<SharedData> > &data)
        {
            extraReferences += data;
        }

        QExplicitlySharedDataPointer<SourceNodeXT> source;
        QExplicitlySharedDataPointer<SinkNodeXT> sink;

    private:
        SourceNode *src;
        SinkNode *snk;
        QList<QExplicitlySharedDataPointer<SharedData> > extraReferences;
};
} // namespace Xine
} // namespace Phonon

/*
inline uint qHash(const Phonon::Xine::WireCall &w)
{
    const uint h1 = qHash(w.source);
    const uint h2 = qHash(w.sink);
    return ((h1 << 16) | (h1 >> 16)) ^ h2;
}
*/

#endif // WIRECALL_H
