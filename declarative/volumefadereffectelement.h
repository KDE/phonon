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

#ifndef PHONON_DECLARATIVE_VOLUMEFADEREFFECTELEMENT_H
#define PHONON_DECLARATIVE_VOLUMEFADEREFFECTELEMENT_H

#include <QtDeclarative/QDeclarativeItem>

#include "abstractinitable.h"
#include "volumefadereffect.h"

namespace Phonon {

namespace Declarative {

/**
 * This is the Qt Quick Element encasing a Phonon::VolumeFaderEffect.
 * For general information regarding capabilities please see the documentation
 * of Phonon::VolumeFaderEffect.
 *
 * Like every Phonon Qt Quick class this class provides semi-lazy initialization
 * as provided described by the AbstractInitAble class.
 *
 * \see Phonon::VolumeFaderEffect
 * \author Harald Sitter <sitter@kde.org>
 */
class VolumeFaderEffectElement : public QObject, public AbstractInitAble
{
    Q_OBJECT
    /// \see Phonon::VolumeFaderEffect::volume
    Q_PROPERTY(float volume READ volume WRITE setVolume)
    /// \see Phonon::VolumeFaderEffect::volumeDecibel
    Q_PROPERTY(double volumeDecibel READ volumeDecibel WRITE setVolumeDecibel)
    /// \see Phonon::VolumeFaderEffect::fadeCurve
    Q_PROPERTY(Phonon::VolumeFaderEffect::FadeCurve fadeCurve READ fadeCurve WRITE setFadeCurve)
    Q_ENUMS(Phonon::VolumeFaderEffect::FadeCurve)
public:
    VolumeFaderEffectElement(QObject *parent = 0);
    ~VolumeFaderEffectElement();

    /// \reimp
    virtual void init(MediaObject *mediaObject);

    float volume() const;
    void setVolume(float volume);

    double volumeDecibel() const;
    void setVolumeDecibel(double volumeDecibel);

    Phonon::VolumeFaderEffect::FadeCurve fadeCurve() const;
    void setFadeCurve(Phonon::VolumeFaderEffect::FadeCurve fadeCurve);

public slots:
    /// \see Phonon::VolumeFaderEffect::fadeIn
    void fadeIn(int time);

    /// \see Phonon::VolumeFaderEffect::fadeOut
    void fadeOut(int time);

    /// \see Phonon::VolumeFaderEffect::fadeTo
    void fadeTo(float volume, int time);

private:
    /**
     * Checks whether the parent item is valid for this element.
     * \returns \c true when parent is either an AudioOutputElement or a
     * MediaElement, \c false otherwise.
     */
    bool isParentValid() const;

    /// The Phonon::VolumeFaderEffect contained by this element.
    VolumeFaderEffect *m_effect;
};

} // namespace Declarative
} // namespace Phonon

#endif // PHONON_DECLARATIVE_VOLUMEFADEREFFECTELEMENT_H
