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

#include "plugin.h"

#include <QtDeclarative/qdeclarative.h>

#include "audiooutputelement.h"
#include "mediaelement.h"
#include "metadata.h"
#include "subtitleelement.h"
#include "videoformatspy.h"
#include "videooutputelement.h"
#include "volumefadereffectelement.h"

#include <QDeclarativeEngine>
#include <QDeclarativeContext>

namespace Phonon
{
namespace Declarative
{

void Plugin::registerTypes(const char *uri)
{
    Q_ASSERT(QLatin1String(uri) == QLatin1String("Phonon"));

    // TODO: version could really be cmake magic?
    qmlRegisterType<Phonon::Declarative::AudioOutputElement>(uri, 1, 0, "AudioOutput");
    qmlRegisterType<Phonon::Declarative::MediaElement>(uri, 1, 0, "Media");
    qmlRegisterType<Phonon::Declarative::MetaData>();
    qmlRegisterType<Phonon::Declarative::SubtitleElement>(uri, 1, 0, "Subtitle");
#ifdef __GNUC__
#warning compat type
#endif
    qmlRegisterType<Phonon::Declarative::VideoOutputElement>(uri, 1, 0, "Video");
    qmlRegisterType<Phonon::Declarative::VideoOutputElement>(uri, 1, 0, "VideoOutput");
    qmlRegisterType<Phonon::Declarative::VolumeFaderEffectElement>(uri, 1, 0, "VolumeFader");
}

} // namespace Declarative
} // namespace Phonon

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
Q_EXPORT_PLUGIN2(PhononDeclarativePlugin, QT_PREPEND_NAMESPACE(Phonon::Declarative::Plugin));
#endif
