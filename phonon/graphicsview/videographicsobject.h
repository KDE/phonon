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

#ifndef PHONON_VIDEOGRAPHICSOBJECT_H
#define PHONON_VIDEOGRAPHICSOBJECT_H

#include <QGraphicsObject>
#include <QMap>

#include "medianode.h"

// FIXME: I really do not want a videoframe include :(
#include "videoframe.h"

// FIXME: review symbol exposure of QGV stuff

namespace Phonon {

class VideoFrame;
class VideoGraphicsFormatSpy;
class VideoGraphicsObjectPrivate;

enum GraphicsPainterType {
    GraphicsPainterNone,
    GraphicsPainterGlsl,
    GraphicsPainterGlArb,
    GraphicsPainterQPainter
};

class PHONON_EXPORT VideoGraphicsPainterMetaFactory
{
public:
    static QMap<GraphicsPainterType, QList<VideoFrame::Format> > detectTypes();
};

/**
 * This class is a video representation implementation for QGraphicsViews/Scenes.
 *
 * It manually obtaines raw frame data from the Phonon backends using the
 * VideoGraphicsObjectInterface and draws them using an implemntation of
 * AbstractVideoGraphicsPainter.
 *
 * The VideoGraphicsObject can be used like any other QGraphicsItem, simply add
 * it to a scene and be done with it.
 * You should however keep in mind that due to the amount of work involved in
 * redrawing an area of screen ~25 times per second that some things possible with
 * other VideoGraphicsItems are not possible with a Phonon VideoGraphicsObject.
 * Also not every painter can deliver all the functionality required for more complex
 * things.
 *
 * For ultimate performance and user experience you should use a Phonon
 * VideoGraphicsObject only in a QGraphicsView with OpenGL viewport. Please take
 * a look at the QGraphicsView documentation for information on how to set an
 * OpenGL viewport.
 *
 * As the QGraphicsView system does not provide implicit size hints you can
 * explicitly request a target geometry for the VideoGraphicsObject using
 * setGeometry().
 *
 * \code
 * QGraphicsView view;
 * QGraphicsScene scene;
 * view.setScene(&scene);
 *
 * Phonon::MediaObject mo;
 * Phonon::VideoGraphicsObject vgo;
 * Phonon::createPath(&mo, &vgo);
 * vgo.setGeometry(QRectF(0,0,320,240));
 * scene.addItem(&vgo);
 * mo.setCurrentSource(Phonon::MediaSource("~/video.ogv"));
 * mo.play();
 *
 * view.show();
 * \endcode
 *
 * \author Harald Sitter <sitter@kde.org>
 */
class PHONON_EXPORT VideoGraphicsObject : public QGraphicsObject, public MediaNode
{
    P_DECLARE_PRIVATE(VideoGraphicsObject)
    Q_OBJECT
public:
    /** Constructor. \param parent the parent of the object. */
    explicit VideoGraphicsObject(QGraphicsItem *parent = 0);

    /** Destructor. */
    virtual ~VideoGraphicsObject();

    /**
     * \returns the rectangle in which the object will paint, either equal
     * or smaller than the set .
     */
    virtual QRectF boundingRect() const;

    /** \param newGeometry the geometry this object should at most occupy. */
    void setGeometry(const QRectF &newGeometry);

    /** \reimp */
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);


    /**
     * Whether the VideoGraphicsObject is ready to begin playback.
     * Usually it would only be ready when it negotiated the format to use. To do
     * this it most of the time requires at least one paint call. Before the VGO
     * is ready a connected MediaObject will refuse to start playback!
     *
     * Should play be called before the VideoGraphicsObject the MediaObject will
     * put the command on a pending list and wait for the ready signal to be
     * emitted.
     *
     * \see ready()
     */
    bool isReady() const;

    // FIXME: q_invoke?
    bool canNegotiate() const;

public slots:
    // FIXME: q_invoke?
    void setSpyFormats(QMap<GraphicsPainterType, QList<VideoFrame::Format> > formats);

signals:
    void ready();

    // FIXME: ew?!
    void gotPaint();

    // TODO: make Q_PRIVATE
private slots:
    void frameReady();
    void reset();

private:
    Q_PRIVATE_SLOT(k_func(), void _p_negotiateFormat())
};

} // namespace Phonon

#endif // PHONON_VIDEOGRAPHICSOBJECT_H
