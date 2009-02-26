/*  This file is part of the KDE project
    Copyright (C) 2006-2007 Matthias Kretz <kretz@kde.org>

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

#ifndef PHONON_VISUALIZATION_H
#define PHONON_VISUALIZATION_H

#include "export.h"
#include "../phonondefs.h"
#include <QtCore/QObject>
#include "../objectdescription.h"
#include "../objectdescriptionmodel.h"
#include "../medianode.h"

namespace Phonon
{
class AudioPath;
class AbstractVideoOutput;

namespace Experimental
{
    class VisualizationPrivate;

enum {
    VisualizationType = 0xfffffffe
};
typedef ObjectDescription<static_cast<Phonon::ObjectDescriptionType>(VisualizationType)> VisualizationDescription;
typedef ObjectDescriptionModel<static_cast<Phonon::ObjectDescriptionType>(VisualizationType)> VisualizationDescriptionModel;

/**
 * \short A class to create visual effects from an audio signal.
 *
 * This class is used to define how an audio signal from an AudioPath object
 * should be visualized. What visualization effects are available depends solely
 * on the backend. You can list the available visualization effects using
 * BackendCapabilities::availableVisualizationEffects().
 *
 * The following example code takes the first effect from the list and uses that
 * to display a visualization on a new VideoWidget.
 * \code
 * QList<VisualizationDescription> list = BackendCapabilities::availableVisualizationEffects();
 * if (list.size() > 0)
 * {
 *   VideoWidget *visWidget = new VideoWidget(parent);
 *   Visualization *vis = new Visualization(visWidget);
 *   vis->setAudioPath(audioPath);
 *   vis->setVideoOutput(visWidget);
 *   vis->setVisualization(list.first());
 * }
 * \endcode
 *
 * \author Matthias Kretz <kretz@kde.org>
 * \see AudioDataOutput
 * \see BackendCapabilities::availableVisualizationEffects()
 */
class PHONONEXPERIMENTAL_EXPORT Visualization : public QObject, public MediaNode
{
    Q_OBJECT
    K_DECLARE_PRIVATE(Visualization)
    PHONON_OBJECT(Visualization)
    Q_PROPERTY(VisualizationDescription visualization READ visualization WRITE setVisualization)

    public:
        ~Visualization();

        VisualizationDescription visualization() const;
        void setVisualization(const VisualizationDescription &newVisualization);

        /**
         * Returns whether the selected visualization effect can be configured
         * by the user with a widget returned by createParameterWidget(). In
         * short it tells you whether createParameterWidget() will return 0 or
         * not.
         */
        //bool hasParameterWidget() const;

        /**
         * Returns a widget that displays effect parameter controls to the user.
         *
         * \param parent The parent widget for the new widget.
         */
        //QWidget *createParameterWidget(QWidget *parent = 0);
};

} // namespace Experimental
} // namespace Phonon

#endif // PHONON_VISUALIZATION_H
// vim: sw=4 ts=4 tw=80
