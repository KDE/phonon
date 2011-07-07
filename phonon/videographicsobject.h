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

#ifndef PHONON_VIDEOGRAPHICSITEM_H
#define PHONON_VIDEOGRAPHICSITEM_H

#include <QtCore/QMutex>
#include <QtGui/QGraphicsItem>

#include "mediaobject.h"

namespace Phonon {

struct VideoFrame {
    /**
     * Video frames come in a variety of formats. Depending on the format
     * you have to process it differently to convert it for displaying or
     * encoding.
     */
    enum Format {
        /**
         * The frame is invalid.
         */
        Format_Invalid = QImage::Format_Invalid,
        /**
         * The frame is stored in data0 using a 24-bit RGB format (8-8-8).
         */
        Format_RGB888 = QImage::Format_RGB888,
        Format_RGB32 = QImage::Format_RGB32,
        /**
         * The frame is stored in data0, data1 and data2 using data0 for the
         * Y data, data1 for the Cb data and data2 for the Cr data.
         *
         * data1 and data2 contain one byte per for adjacent pixels whereas data0
         * has one byte per pixel.
         */
        Format_YCbCr420 = 0x10000,
        Format_YV12 = Format_YCbCr420,
        /**
         * The frame is stored in data0 using a 32-bit Y0-Cb-Y1-Cr format (8-8-8-8).
         */
        Format_YCbCr422 = 0x10001,
        Format_YUY2 = Format_YCbCr422
    };

    /**
     * The width of the video frame in number of pixels.
     */
    int width;

    /**
     * The height of the video frame in number of pixels.
     */
    int height;

    /**
     * The aspect ratio the frame should be displayed with.
     *
     * Common values are 4/3, 16/9.
     */
    double aspectRatio;

    /**
     * Convenience function to calculate the aspect corrected width from the
     * aspectRatio and height values.
     *
     * It is recommended to display video frames with aspectCorrectedWidth x height
     */
    inline int aspectCorrectedWidth() const { return qRound(aspectRatio * height); }

    /**
     * Convenience function to calculate the aspect corrected height from the
     * aspectRatio and width values.
     *
     * It is recommended to display video frames with aspectCorrectedWidth x height
     */
    inline int aspectCorrectedHeight() const { return qRound(width / aspectRatio); }

    /**
     * Format of the frame.
     *
     * \see Format
     */
    Format format;

    /**
     * RGB8, YUY2 or Y-plane
     *
     * If format is Format_RGB888 then the data contains each pixel as three
     * consecutive bytes for red, green and blue.
     *
     * If format is Format_YUY2 the data contains every two pixels as four
     * consecutive bytes for Y0, Cb, Y1, Cr
     *
     * If format is Format_YV12 the data contains one byte per pixel with
     * the Y value.
     */
    QByteArray data0;

    /**
     * YV12 U-plane
     *
     * If format is Format_YV12 the data contains one byte per four adjacent
     * pixels with the Cb value.
     */
    QByteArray data1;

    /**
     * YV12 V-plane
     *
     * If format is Format_YV12 the data contains one byte per four adjacent
     * pixels with the Cr value.
     */
    QByteArray data2;

    inline QImage qImage() const
    {
        if (format == Format_RGB888) {
            return QImage(reinterpret_cast<const uchar *>(data0.constData()),
                          width, height, QImage::Format_RGB888);
        }
        return QImage();
    }
};

class PHONON_EXPORT VideoGraphicsObject : public QGraphicsObject
{
    Q_OBJECT
    friend class VideoGraphicsObjectInterface;
public:
    explicit VideoGraphicsObject(QGraphicsItem *parent = 0);
    virtual ~VideoGraphicsObject();

    virtual QRectF boundingRect() const { return QRectF(0, 0, 320, 240); return m_rect; }

    void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);
    virtual void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry);

    void setTargetRect();

protected slots:
    void setFrame(const VideoFrame &frame);

private:
    VideoFrame  m_frame;

    QMutex m_mutex;
    QRectF m_rect;
    QSize m_frameSize;
    QSizeF m_targetSize;

    QObject *m_backendObject;
};

class VideoGraphicsObjectInterface
{
public:
    virtual ~VideoGraphicsObjectInterface() {}

    virtual VideoGraphicsObject *videoGraphicsObject() = 0;
    virtual void setVideoGraphicsObject(VideoGraphicsObject *object) = 0;
};

} // namespace Phonon

Q_DECLARE_INTERFACE(Phonon::VideoGraphicsObjectInterface, "VideoGraphicsObjectInterface.phonon.kde.org")
Q_DECLARE_METATYPE(Phonon::VideoFrame)

#endif // PHONON_VIDEOGRAPHICSITEM_H
