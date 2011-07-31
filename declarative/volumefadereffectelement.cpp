#include "volumefadereffectelement.h"

#include "audiooutputelement.h"
#include "volumefadereffect.h"

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
    m_effect->setFadeCurve(Phonon::VolumeFaderEffect::Fade9Decibel);
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

void VolumeFaderEffectElement::fadeOut(int time)
{
    qDebug() << Q_FUNC_INFO;
    m_effect->fadeOut(time);
}

} // namespace Declarative
} // namespace Phonon
