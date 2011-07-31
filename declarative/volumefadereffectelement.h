#ifndef PHONON_DECLARATIVE_VOLUMEFADEREFFECTELEMENT_H
#define PHONON_DECLARATIVE_VOLUMEFADEREFFECTELEMENT_H

#include <QtDeclarative/QDeclarativeItem>

#include "abstractinitable.h"

namespace Phonon {

class VolumeFaderEffect;

namespace Declarative {

class VolumeFaderEffectElement : public QDeclarativeItem,
                                 public AbstractInitAble
{
    Q_OBJECT
public:
    explicit VolumeFaderEffectElement(QDeclarativeItem *parent = 0);
    ~VolumeFaderEffectElement();

    void classBegin();

    virtual void init(MediaObject *mediaObject);

public slots:
    void fadeOut(int time);

private:
    VolumeFaderEffect *m_effect;
};

} // namespace Declarative
} // namespace Phonon

#endif // PHONON_DECLARATIVE_VOLUMEFADEREFFECTELEMENT_H
