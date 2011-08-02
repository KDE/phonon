#include "volumefadereffectelement.h"

#include "audiooutputelement.h"

namespace Phonon {
namespace Declarative {

VolumeFaderEffectElement::VolumeFaderEffectElement(QDeclarativeItem *parent) :
    QDeclarativeItem(parent)
{
}

VolumeFaderEffectElement::~VolumeFaderEffectElement()
{
}

void VolumeFaderEffectElement::classBegin()
{
    m_effect = new VolumeFaderEffect(this);
}

void VolumeFaderEffectElement::init(MediaObject *mediaObject)
{
    Q_ASSERT(mediaObject);

    AudioOutputElement *audioElement = qobject_cast<AudioOutputElement *>(parentItem());
    Q_ASSERT(audioElement);

    audioElement->m_path.insertEffect(m_effect);
    m_effect->setVolume(1.0);
    m_effect->fadeTo(1.0, 0);
}

float VolumeFaderEffectElement::volume() const
{
    return m_effect->volume();
}

void VolumeFaderEffectElement::setVolume(float volume)
{
    m_effect->setVolume(volume);
}

double VolumeFaderEffectElement::volumeDecibel() const
{
    return m_effect->volumeDecibel();
}

void VolumeFaderEffectElement::setVolumeDecibel(double volumeDecibel)
{
    m_effect->setVolumeDecibel(volumeDecibel);
}

VolumeFaderEffect::FadeCurve VolumeFaderEffectElement::fadeCurve() const
{
    return m_effect->fadeCurve();
}

void VolumeFaderEffectElement::setFadeCurve(VolumeFaderEffect::FadeCurve fadeCurve)
{
    m_effect->setFadeCurve(fadeCurve);
}

void VolumeFaderEffectElement::fadeIn(int time)
{
    qDebug() << Q_FUNC_INFO;
    m_effect->fadeIn(time);
}

void VolumeFaderEffectElement::fadeOut(int time)
{
    qDebug() << Q_FUNC_INFO;
    m_effect->fadeOut(time);
}

void VolumeFaderEffectElement::fadeTo(float volume, int time)
{
    qDebug() << Q_FUNC_INFO;
    m_effect->fadeTo(volume, time);
}

} // namespace Declarative
} // namespace Phonon
