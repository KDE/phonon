#ifndef PHONON_VIDEOSURFACEINTERFACE_H
#define PHONON_VIDEOSURFACEINTERFACE_H

#include <QSize>

namespace Phonon {

struct VideoFrame {
    enum Format {
        Format_Invalid, /** < Invalid Frame */
        Format_RGB32,   /** < Packed RGB32 */
        Format_YV12,    /** < 3 planes 8 bit Y plane followed by 8 bit 2x2
                              subsampled V and U planes. */
        Format_I420     /** < Like YV12, but U and V are swapped */
    };

    VideoFrame()
        : width(0)
        , height(0)
        , format(Format_Invalid)
    {
        for (int i = 0; i < 4; ++i) {
            pitch[i] = 0;
            visiblePitch[i] = 0;
            lines[i] = 0;
            visibleLines[i] = 0;
        }
    }

    unsigned int width;
    unsigned int height;
#warning compat
    QSize size() const { return QSize(width, height); }
    Format format;
#warning compat
    bool isValid() const { return format != Format_Invalid; }
    unsigned int planeCount;
    QByteArray plane[4];

    /**
     * Pitch/stride of a plane.
     * This pitch is aligned at the backend's desire, so you must not assume any
     * particular alignment.
     * @note The pitch is always bigger than the visible pitch, for painting you
     *       want to clip the plane accordingly (e.g. treat the visible pitch as
     *       a subpicture).
     * @note for conversion into a QImage you will have to pass the appropriate
     *       pitch as bytesPerLine or you'll get unexpected results.
     */
    int pitch[4];

    /**
     * The visible pitch is the actually visible pitch of a plane.
     * It depends only on the format (e.g. for YV12 0 = width, 1 = 2 = width/2).
     */
    int visiblePitch[4];

    /**
     * Lines/scanlines in the plane (aligned).
     * @see pitch
     */
    int lines[4];

    /**
     * Visible lines/scanlines in the plane.
     * @see visiblePitch
     */
    int visibleLines[4];
};

class VideoSurfaceOutputInterface
{
public:
    virtual bool tryLock() = 0;
    virtual void lock() = 0;
    virtual void unlock() = 0;
    virtual const VideoFrame *frame() const = 0;

//Q_SIGNALS:
    virtual void frameReady() = 0;
};

} // namespace Phonon

Q_DECLARE_INTERFACE(Phonon::VideoSurfaceOutputInterface, "org.kde.phonon.VideoSurfaceOutputInterface/5.0")

#endif // PHONON_VIDEOSURFACEINTERFACE_H
