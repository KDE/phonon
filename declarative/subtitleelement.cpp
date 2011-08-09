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

#include "subtitleelement.h"

#include <QtCore/QStringList>

#include "phonon/mediacontroller.h"

namespace Phonon {
namespace Declarative {

#define PROTECTOR(returnType) if (!m_mediaController) return returnType()

SubtitleElement::SubtitleElement(QObject *parent) :
    QObject(parent),
    AbstractInitAble()
{
}

void SubtitleElement::init(MediaObject *mediaObject)
{
#warning inheritance bad -> inits all the phonon without being used
    Q_ASSERT(mediaObject);

    m_mediaController = new MediaController(mediaObject);
    connect(m_mediaController, SIGNAL(availableSubtitlesChanged()),
            this, SIGNAL(availableSubtitlesChanged()));
}

QStringList SubtitleElement::availableSubtitles() const
{
    PROTECTOR(QStringList);
    QList<SubtitleDescription> descriptors = m_mediaController->availableSubtitles();
    QStringList list;
    foreach (const SubtitleDescription &descriptor, descriptors) {
        list.append(descriptor.name());
    }
    return list;
}

QString SubtitleElement::subtitle() const
{
    PROTECTOR(QString);
    return m_mediaController->currentSubtitle().name();
}

void SubtitleElement::setSubtitle(const QString &subtitle)
{
    PROTECTOR(void);
    qDebug() << "trying to set" << subtitle;
    QList<SubtitleDescription> descriptors = m_mediaController->availableSubtitles();
    foreach (const SubtitleDescription &descriptor, descriptors) {
        if (descriptor.name() == subtitle) {
            qDebug() << "matched:" << descriptor;
            m_mediaController->setCurrentSubtitle(descriptor);
            emit subtitleChanged();
            return;
        }
    }
    qDebug() << "no sub for subtitle" << subtitle;
    qWarning("did not find subtitle descriptor");
}

} // namespace Declarative
} // namespace Phonon
