#ifndef PHONON_VIDEOITEM_H
#define PHONON_VIDEOITEM_H

#include <QQuickItem>

#include "abstractvideooutput.h"

namespace Phonon {

class VideoItemPrivate;
class PHONON_EXPORT VideoItem : public QQuickItem, public AbstractVideoOutput
{
    Q_OBJECT
public:
    explicit VideoItem(QQuickItem *parent = 0);
    ~VideoItem();

private slots:
    void onFrameReady();

protected:
    virtual QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *updatePaintNodeData);

private:
    P_DECLARE_PRIVATE(VideoItem)
};

} // namespace Phonon

#endif // PHONON_VIDEOITEM_H
