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

#ifndef PHONON_XINE_EVENTS_H
#define PHONON_XINE_EVENTS_H

#include "wirecall.h"
#include "xinestream.h"

#include <QtCore/QEvent>
#include <QtCore/QSize>
#include <QtCore/QPair>
#include <QtCore/QList>
#include <QtGui/QImage>

#define QEVENT(type) Event(Event::type)

#define EVENT_CLASS0(type) \
class type##Event : public Event \
{ \
    public: \
        inline type##Event() : QEVENT(type) {} \
}; \
template <> inline type##Event *copyEvent<type##Event>(type##Event *) \
{ \
    return new type##Event(); \
}
#define EVENT_CLASS1(type, arg1, init1, member1type, member1name) \
class type##Event : public Event \
{ \
    public: \
        inline type##Event(arg1) : QEVENT(type), init1 {} \
        member1type member1name; \
}; \
template <> inline type##Event *copyEvent<type##Event>(type##Event *e) \
{ \
    return new type##Event(static_cast<type##Event *>(e)->member1name); \
}
#define EVENT_CLASS2(type, arg1, arg2, init1, init2, member1type, member1name, member2type, member2name) \
class type##Event : public Event \
{ \
    public: \
        inline type##Event(arg1, arg2) : QEVENT(type), init1, init2 {} \
        member1type member1name; \
        member2type member2name; \
}; \
template <> inline type##Event *copyEvent<type##Event>(type##Event *e) \
{ \
    return new type##Event(static_cast<type##Event *>(e)->member1name, static_cast<type##Event *>(e)->member2name); \
}
namespace Phonon
{
namespace Xine
{


class Event : public QEvent
{
public:
    enum Type {
        GetStreamInfo = 2001,
        UpdateVolume,
        RewireVideoToNull,
        PlayCommand,
        PauseCommand,
        StopCommand,
        SeekCommand,
        MrlChanged,
        TransitionTypeChanged,
        GaplessSwitch,
        UpdateTime,
        SetTickInterval,
        SetPrefinishMark,
        SetParam,
        EventSend,
        QuitLoop,
        PauseForBuffering,
        UnpauseForBuffering,
        Error,
        NewStream,
        NewMetaData,
        MediaFinished,
        Progress,
        NavButtonIn,
        NavButtonOut,
        AudioDeviceFailed,
        FrameFormatChange,
        UiChannelsChanged,
        Reference,
        Rewire,
        HasVideo,
        IsThereAXineEngineForMe,
        NoThereIsNoXineEngineForYou,
        HeresYourXineStream,
        Cleanup,
        RequestSnapshot,
        SnapshotReady
    };

    int ref;

    inline Event(Type t) : QEvent(static_cast<QEvent::Type>(t)), ref(1) {}

    inline Type type() const { return static_cast<Type>(QEvent::type()); }
}; // class Event

template<typename T>
inline T *copyEvent(T *)
{
    abort();
    return 0;
}

EVENT_CLASS1(SnapshotReady, QImage i, image(i), const QImage, image)
EVENT_CLASS1(HeresYourXineStream, QExplicitlySharedDataPointer<XineStream> s, stream(s), QExplicitlySharedDataPointer<XineStream>, stream)
EVENT_CLASS1(HasVideo, bool v, hasVideo(v), const bool, hasVideo)
EVENT_CLASS1(UpdateVolume, int v, volume(v), const int, volume)
EVENT_CLASS1(EventSend, const xine_event_t *const e, event(e), const xine_event_t *const, event)
EVENT_CLASS1(GaplessSwitch, const QByteArray &_mrl, mrl(_mrl), const QByteArray, mrl)
EVENT_CLASS1(SetTickInterval, qint32 i, interval(i), const qint32, interval)
EVENT_CLASS1(SetPrefinishMark, qint32 i, time(i), const qint32, time)

EVENT_CLASS2(Rewire, QList<WireCall> _wireCalls, QList<WireCall> _unwireCalls, wireCalls(_wireCalls), unwireCalls(_unwireCalls), const QList<WireCall>, wireCalls, const QList<WireCall>, unwireCalls)
EVENT_CLASS2(Reference, bool alt, const QByteArray &m, alternative(alt), mrl(m), const bool, alternative, const QByteArray, mrl)
EVENT_CLASS2(Progress, const QString &d, int p, description(d), percent(p), const QString, description, const int, percent)
EVENT_CLASS2(Error, Phonon::ErrorType t, const QString &r, type(t), reason(r), const Phonon::ErrorType, type, const QString, reason)
EVENT_CLASS2(SetParam, int p, int v, param(p), value(v), const int, param, const int, value)
EVENT_CLASS2(MrlChanged, const QByteArray &_mrl, XineStream::StateForNewMrl _s, mrl(_mrl), stateForNewMrl(_s), const QByteArray, mrl, const XineStream::StateForNewMrl, stateForNewMrl)

class FrameFormatChangeEvent : public Event
{
    public:
        FrameFormatChangeEvent(int w, int h, int a, bool ps)
            : QEVENT(FrameFormatChange),
            size(w, h), aspect(a), panScan(ps) {}

        const QSize size;
        const int aspect;
        const bool panScan;
};
template <> inline FrameFormatChangeEvent *copyEvent<FrameFormatChangeEvent>(FrameFormatChangeEvent *e)
{
    return new FrameFormatChangeEvent(static_cast<FrameFormatChangeEvent *>(e)->size.width(), static_cast<FrameFormatChangeEvent *>(e)->size.height(), static_cast<FrameFormatChangeEvent *>(e)->aspect, static_cast<FrameFormatChangeEvent *>(e)->panScan);
}

class SeekCommandEvent : public Event
{
    public:
        SeekCommandEvent(qint64 t) : QEVENT(SeekCommand), time(t) {}
        const qint64 time;
};
template <> inline SeekCommandEvent *copyEvent<SeekCommandEvent>(SeekCommandEvent *e)
{
    return new SeekCommandEvent(static_cast<SeekCommandEvent *>(e)->time);
}

} // namespace Xine
} // namespace Phonon

#undef EVENT_CLASS1
#undef EVENT_CLASS2

#endif // PHONON_XINE_EVENTS_H
