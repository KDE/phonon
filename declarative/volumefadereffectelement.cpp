#include "volumefadereffectelement.h"

#include <QtDeclarative/QDeclarativeInfo>

#include "audiooutputelement.h"
#include "mediaelement.h"

namespace Phonon {
namespace Declarative {

#define PROTECT(ret) P_D_PROTECT(m_effect, ret)
#define PROTECT_VALUE(ret) P_D_PROTECT_VALUE(m_effect, ret)

VolumeFaderEffectElement::VolumeFaderEffectElement(QObject *parent) :
    QObject(parent)
{
}

VolumeFaderEffectElement::~VolumeFaderEffectElement()
{
}

void VolumeFaderEffectElement::init(MediaObject *mediaObject)
{
    Q_ASSERT(mediaObject);

    AudioOutputElement *audioElement = qobject_cast<AudioOutputElement *>(parent());

    if (!isParentValid()) {
        qmlInfo(this) << "A VolumeFaderEffect item may only be used as child of a Media or "
                      << "Audio element\nVolumeFaderEffect item can not be initialized.";
    }

    m_effect = new VolumeFaderEffect(this);
    audioElement->m_path.insertEffect(m_effect);
    m_effect->setVolume(1.0);
    m_effect->fadeTo(1.0, 0);
}

float VolumeFaderEffectElement::volume() const
{
    PROTECT_VALUE(0);
    return m_effect->volume();
}

void VolumeFaderEffectElement::setVolume(float volume)
{
    PROTECT(void);
    m_effect->setVolume(volume);
}

double VolumeFaderEffectElement::volumeDecibel() const
{
    PROTECT_VALUE(0);
    return m_effect->volumeDecibel();
}

void VolumeFaderEffectElement::setVolumeDecibel(double volumeDecibel)
{
    PROTECT(void);
    m_effect->setVolumeDecibel(volumeDecibel);
}

VolumeFaderEffect::FadeCurve VolumeFaderEffectElement::fadeCurve() const
{
    PROTECT_VALUE(VolumeFaderEffect::Fade9Decibel);
    return m_effect->fadeCurve();
}

void VolumeFaderEffectElement::setFadeCurve(VolumeFaderEffect::FadeCurve fadeCurve)
{
    PROTECT(void);
    m_effect->setFadeCurve(fadeCurve);
}

void VolumeFaderEffectElement::fadeIn(int time)
{
    PROTECT(void);
    qDebug() << Q_FUNC_INFO;
    m_effect->fadeIn(time);
}

void VolumeFaderEffectElement::fadeOut(int time)
{
    PROTECT(void);
    qDebug() << Q_FUNC_INFO;
    m_effect->fadeOut(time);
}

void VolumeFaderEffectElement::fadeTo(float volume, int time)
{
    PROTECT(void);
    qDebug() << Q_FUNC_INFO;
    m_effect->fadeTo(volume, time);
}

bool VolumeFaderEffectElement::isParentValid() const
{
    if (!parent() ||
            (!qobject_cast<AudioOutputElement *>(parent()) &&
             !qobject_cast<MediaElement *>(parent()))) {
        return false;
    }
    return true;
}

} // namespace Declarative
} // namespace Phonon
