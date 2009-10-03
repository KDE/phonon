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
#ifndef PHONON_XINE_EFFECT_H
#define PHONON_XINE_EFFECT_H

#include <QObject>
#include <phonon/effectparameter.h>
#include <phonon/effectinterface.h>
#include <QList>
#include <xine.h>
#include <QMutex>
#include "sinknode.h"
#include "sourcenode.h"
#include "xinestream.h"

namespace Phonon
{
namespace Xine
{
class Effect;

class EffectXT : public SourceNodeXT, public SinkNodeXT
{
    friend class Effect;
    public:
        EffectXT(const char *name);
        ~EffectXT();
        xine_audio_port_t *audioPort() const;
        xine_post_out_t *audioOutputPort() const;
        void rewireTo(SourceNodeXT *source);
        virtual void createInstance();
    protected:
        xine_audio_port_t *fakeAudioPort();

        xine_post_t *m_plugin;
        xine_post_api_t *m_pluginApi;

    private:
        void ensureInstance();

        xine_audio_port_t *m_fakeAudioPort;
        mutable QMutex m_mutex;
        const char *m_pluginName;
        char *m_pluginParams;
        QList<Phonon::EffectParameter> m_parameterList;
};

class Effect : public QObject, public EffectInterface, public SinkNode, public SourceNode
{
    Q_OBJECT
    Q_INTERFACES(Phonon::EffectInterface Phonon::Xine::SinkNode Phonon::Xine::SourceNode)
    public:
        Effect(int effectId, QObject *parent);

        bool isValid() const;

        MediaStreamTypes inputMediaStreamTypes() const;
        MediaStreamTypes outputMediaStreamTypes() const;
        SourceNode *sourceInterface() { return this; }
        SinkNode *sinkInterface() { return this; }

        QList<EffectParameter> parameters() const;

        QVariant parameterValue(const EffectParameter &p) const;
        void setParameterValue(const EffectParameter &p, const QVariant &newValue);

    protected:
        void aboutToChangeXineEngine();
        void xineEngineChanged();
        virtual void ensureParametersReady();
        Effect(EffectXT *, QObject *parent);
        void addParameter(const EffectParameter &p);
};
}} //namespace Phonon::Xine

// vim: sw=4 ts=4
#endif // PHONON_XINE_EFFECT_H
