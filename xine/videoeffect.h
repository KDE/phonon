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
#ifndef Phonon_XINE_VIDEOEFFECT_H
#define Phonon_XINE_VIDEOEFFECT_H

#include <QObject>
#include <phonon/effectparameter.h>
#include <QList>
#include <xine.h>
#include <QMutex>

namespace Phonon
{
namespace Xine
{
class VideoPath;

class VideoEffect : public QObject
{
    Q_OBJECT
    public:
        VideoEffect(int effectId, QObject *parent);
        ~VideoEffect();

        virtual void setPath(VideoPath *);

        virtual xine_post_t *newInstance(xine_video_port_t *);

    public slots:
        QList<EffectParameter> allDescriptions();
        EffectParameter description(int parameterIndex);
        int parameterCount();

        QVariant parameterValue(int parameterIndex) const;
        void setParameterValue(int parameterIndex, const QVariant &newValue);

    protected:
        VideoPath *path() const { return m_path; }
        virtual void ensureParametersReady();
        VideoEffect(const char *name, QObject *parent);
        void addParameter(const EffectParameter &p) { m_parameterList << p; }

        QList<xine_post_t *> m_plugins;
        QList<xine_post_api_t *> m_pluginApis;

    private:
        mutable QMutex m_mutex;
        const char *m_pluginName;
        char *m_pluginParams;
        VideoPath *m_path;
        QList<Phonon::EffectParameter> m_parameterList;
};
}} //namespace Phonon::Xine

// vim: sw=4 ts=4 tw=80
#endif // Phonon_XINE_VIDEOEFFECT_H
