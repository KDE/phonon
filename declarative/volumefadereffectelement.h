#ifndef PHONON_DECLARATIVE_VOLUMEFADEREFFECTELEMENT_H
#define PHONON_DECLARATIVE_VOLUMEFADEREFFECTELEMENT_H

#include <QtDeclarative/QDeclarativeItem>

#include "abstractinitable.h"
#include "volumefadereffect.h"

namespace Phonon {

namespace Declarative {

class VolumeFaderEffectElement : public QObject,
                                 public AbstractInitAble
{
    Q_OBJECT
    Q_PROPERTY(float volume READ volume WRITE setVolume)
    Q_PROPERTY(double volumeDecibel READ volumeDecibel WRITE setVolumeDecibel)
    Q_PROPERTY(Phonon::VolumeFaderEffect::FadeCurve fadeCurve READ fadeCurve WRITE setFadeCurve)
    Q_ENUMS(Phonon::VolumeFaderEffect::FadeCurve)
public:
    explicit VolumeFaderEffectElement(QObject *parent = 0);
    ~VolumeFaderEffectElement();

    virtual void init(MediaObject *mediaObject);

    float volume() const;
    void setVolume(float volume);

    double volumeDecibel() const;
    void setVolumeDecibel(double volumeDecibel);

    Phonon::VolumeFaderEffect::FadeCurve fadeCurve() const;
    void setFadeCurve(Phonon::VolumeFaderEffect::FadeCurve fadeCurve);

public slots:
    void fadeIn(int time);
    void fadeOut(int time);

    void fadeTo(float volume, int time);

private:
    bool isParentValid() const;

    VolumeFaderEffect *m_effect;
};

} // namespace Declarative
} // namespace Phonon

#endif // PHONON_DECLARATIVE_VOLUMEFADEREFFECTELEMENT_H
