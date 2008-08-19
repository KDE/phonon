/*  This file is part of the KDE project
    Copyright (C) 2007 Matthias Kretz <kretz@kde.org>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.

*/

#include <kdemacros.h>
#include <phonon/volumefadereffect.h>
#include <klocale.h>
#include <kdebug.h>
#include <kglobal.h>
#include <cmath>

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include <xine.h>
extern "C" {
// xine headers use the reserved keyword this:
#define this this_xine
#include <xine/compat.h>
#include <xine/post.h>
#include <xine/xine_internal.h>
#include <xine/xineutils.h>
#undef this

typedef struct
{
    post_class_t post_class;
    xine_t *xine;
} kvolumefader_class_t;

typedef struct KVolumeFaderPlugin
{
    post_plugin_t post;

    /* private data */
    pthread_mutex_t    lock;
    xine_post_in_t params_input;

    int rate;

    Phonon::VolumeFaderEffect::FadeCurve fadeCurve;
    float fadeStart;
    float fadeDiff;
    int fadeTime;
    int curvePosition;
    int curveLength;
    float oneOverCurveLength;

    float (*curveValue)(const float &fadeStart, const float &fadeDiff, const int &position, const float &length);
    void fadeBuffer(audio_buffer_t *buf);
} kvolumefader_plugin_t;

/**************************************************************************
 * curve functions
 *************************************************************************/

static const float maxVolume = 1.0f;

/*
 * power = voltage²
 *
 * let max. voltage = 1.0
 * => max. power = 1.0
 *
 * let log(power=0.5) = -3dB
 *                                                               ___
 * -3dB  =   log(0.5)             => power = 0.5   => voltage = √0.5
 *
 * -6dB  = 2 *log(0.5) = log(0.5²) => power = 0.25  => voltage = 0.5
 *                                                               ____
 * -9dB  = 3 *log(0.5) = log(0.5³) => power = 0.125 => voltage = √0.5³
 *
 * -12dB = 4 *log(0.5) = log(0.5⁴) => power = 0.5⁴  => voltage = 0.5²
 */
static float curveValueFadeIn3dB(const float &fadeStart, const float &fadeDiff, const int &position, const float &length)
{
    return (fadeStart + fadeDiff * sqrt(static_cast<double>(position) * length));
}
static float curveValueFadeOut3dB(const float &fadeStart, const float &fadeDiff, const int &position, const float &length)
{
    return (fadeStart + fadeDiff * (1.0 - sqrt(1.0 - static_cast<double>(position) * length)));
}
// in == out for a linear fade
static float curveValueFade6dB(const float &fadeStart, const float &fadeDiff, const int &position, const float &length)
{
    return (fadeStart + fadeDiff * position * length);
}
static float curveValueFadeIn9dB(const float &fadeStart, const float &fadeDiff, const int &position, const float &length)
{
    return (fadeStart + fadeDiff * pow(static_cast<double>(position) * length, 1.5));
}
static float curveValueFadeOut9dB(const float &fadeStart, const float &fadeDiff, const int &position, const float &length)
{
    return (fadeStart + fadeDiff * (1.0 - pow(1.0 - static_cast<double>(position) * length, 1.5)));
}
static float curveValueFadeIn12dB(const float &fadeStart, const float &fadeDiff, const int &position, const float &length)
{
    const float x = position * length;
    return (fadeStart + fadeDiff * x * x);
}
static float curveValueFadeOut12dB(const float &fadeStart, const float &fadeDiff, const int &position, const float &length)
{
    const float x = 1.0f - position * length;
    return (fadeStart + fadeDiff * (1.0 - x * x));
}

/**************************************************************************
 * parameters
 *************************************************************************/

typedef struct
{
    Phonon::VolumeFaderEffect::FadeCurve fadeCurve;
    double currentVolume;
    double fadeTo;
    int fadeTime;
} kvolumefader_parameters_t;

/*
 * description of params struct
 */
static char *enum_fadeCurve[] = { "Fade3Decibel", "Fade6Decibel", "Fade9Decibel", "Fade12Decibel", NULL };
START_PARAM_DESCR(kvolumefader_parameters_t)
PARAM_ITEM(POST_PARAM_TYPE_INT, fadeCurve, enum_fadeCurve, 0.0, 0.0, 0, I18N_NOOP("fade curve"))
PARAM_ITEM(POST_PARAM_TYPE_DOUBLE, currentVolume, NULL, 0.0, maxVolume, 0, I18N_NOOP("current volume"))
PARAM_ITEM(POST_PARAM_TYPE_DOUBLE, fadeTo, NULL, 0.0, maxVolume, 0, I18N_NOOP("volume to fade to"))
PARAM_ITEM(POST_PARAM_TYPE_INT, fadeTime, NULL, 0.0, 10000.0, 0, I18N_NOOP("fade time in milliseconds"))
END_PARAM_DESCR(param_descr)

static int set_parameters (xine_post_t *this_gen, void *param_gen) 
{
    kvolumefader_plugin_t *that = reinterpret_cast<kvolumefader_plugin_t *>(this_gen);
    kvolumefader_parameters_t *param = static_cast<kvolumefader_parameters_t *>(param_gen);

    pthread_mutex_lock (&that->lock);
    that->fadeCurve = param->fadeCurve;
    that->fadeStart = param->currentVolume;
    that->fadeDiff = param->fadeTo - that->fadeStart;
    that->curvePosition = 0;
    that->fadeTime = param->fadeTime;
    that->curveLength = static_cast<int>((param->fadeTime * that->rate) / 1000);
    that->oneOverCurveLength = 1000.0f / (param->fadeTime * that->rate);
    const char *x = "unknown";
    switch (that->fadeCurve) {
    case Phonon::VolumeFaderEffect::Fade3Decibel:
        if (that->fadeDiff > 0) {
            that->curveValue = curveValueFadeIn3dB;
        } else {
            that->curveValue = curveValueFadeOut3dB;
        }
        x = "3dB";
        break;
    case Phonon::VolumeFaderEffect::Fade6Decibel:
        that->curveValue = curveValueFade6dB;
        x = "6dB";
        break;
    case Phonon::VolumeFaderEffect::Fade9Decibel:
        if (that->fadeDiff > 0) {
            that->curveValue = curveValueFadeIn9dB;
        } else {
            that->curveValue = curveValueFadeOut9dB;
        }
        x = "9dB";
        break;
    case Phonon::VolumeFaderEffect::Fade12Decibel:
        if (that->fadeDiff > 0) {
            that->curveValue = curveValueFadeIn12dB;
        } else {
            that->curveValue = curveValueFadeOut12dB;
        }
        x = "12dB";
        break;
    }
    kDebug(610)
        << x
        << param->currentVolume
        << param->fadeTo
        << param->fadeTime << "=>"
        << that->fadeStart
        << that->fadeDiff
        << that->curvePosition
        << that->oneOverCurveLength
        ;
    pthread_mutex_unlock (&that->lock);

    return 1;
}

static int get_parameters (xine_post_t *this_gen, void *param_gen) 
{
    kvolumefader_plugin_t *that = reinterpret_cast<kvolumefader_plugin_t *>(this_gen);
    kvolumefader_parameters_t *param = static_cast<kvolumefader_parameters_t *>(param_gen);

    pthread_mutex_lock (&that->lock);
    param->fadeCurve = that->fadeCurve;
    if (that->curvePosition == 0) {
        param->currentVolume = that->fadeStart;
    } else {
        param->currentVolume = that->curveValue(that->fadeStart, that->fadeDiff, that->curvePosition, that->oneOverCurveLength);
    }
    param->fadeTo = that->fadeDiff + that->fadeStart;
    param->fadeTime = that->fadeTime;
    pthread_mutex_unlock (&that->lock);

    return 1;
}

static xine_post_api_descr_t *get_param_descr()
{
    return &param_descr;
}

static char *get_help ()
{
    static QByteArray helpText(
            i18n("Normalizes audio by maximizing the volume without distorting "
                 "the sound.\n"
                 "\n"
                 "Parameters:\n"
                 "  method: 1: use a single sample to smooth the variations via "
                 "the standard weighted mean over past samples (default); 2: use "
                 "several samples to smooth the variations via the standard "
                 "weighted mean over past samples.\n").toUtf8());
    return helpText.data();
}

static xine_post_api_t post_api = {
    set_parameters,
    get_parameters,
    get_param_descr,
    get_help,
};


/**************************************************************************
 * xine audio post plugin functions
 *************************************************************************/

static int kvolumefader_port_open(xine_audio_port_t *port_gen, xine_stream_t *stream,
                             uint32_t bits, uint32_t rate, int mode)
{
    post_audio_port_t *port = reinterpret_cast<post_audio_port_t *>(port_gen);
    kvolumefader_plugin_t *that = reinterpret_cast<kvolumefader_plugin_t *>(port->post);

    _x_post_rewire(&that->post);
    _x_post_inc_usage(port);

    port->stream = stream;
    port->bits = bits;
    port->rate = rate;
    port->mode = mode;
    that->rate = rate;
    switch (mode) {
    case AO_CAP_MODE_STEREO:
        that->rate *= 2;
        break;
    case AO_CAP_MODE_4CHANNEL:
        that->rate *= 4;
        break;
    case AO_CAP_MODE_4_1CHANNEL:
    case AO_CAP_MODE_5CHANNEL:
    case AO_CAP_MODE_5_1CHANNEL:
        that->rate *= 6;
        break;
    }
    that->curveLength = static_cast<int>((that->fadeTime * that->rate) / 1000);
    that->oneOverCurveLength = 1000.0f / (that->fadeTime * that->rate);

    return port->original_port->open(port->original_port, stream, bits, rate, mode);
}

static void kvolumefader_port_close(xine_audio_port_t *port_gen, xine_stream_t *stream)
{
    post_audio_port_t *port = reinterpret_cast<post_audio_port_t *>(port_gen);

    port->stream = NULL;
    port->original_port->close(port->original_port, stream);
    _x_post_dec_usage(port);
}

void KVolumeFaderPlugin::fadeBuffer(audio_buffer_t *buf)
{
    const int num_channels = _x_ao_mode2channels(buf->format.mode);
    const int bufferLength = buf->num_frames * num_channels;
    if (buf->format.bits == 16 || buf->format.bits == 0) {
        //kDebug(610) 
            //<< " bufferLength = " << bufferLength
            //<< " start = " << fadeStart
            //<< " diff = " << fadeDiff
            //<< " pos = " << curvePosition
            //<< " curveLength = " << oneOverCurveLength
            //<< " curveValue = " << curveValue()
            //;
        int16_t *data = static_cast<int16_t *>(buf->mem);
        int i = 0;
        for (; curvePosition < curveLength && i < bufferLength; ++i, ++curvePosition) {
            data[i] = static_cast<int16_t>(data[i] * curveValue(fadeStart, fadeDiff, curvePosition, oneOverCurveLength));
        }
        if (curveLength > 0 && curvePosition >= curveLength) {
            curveLength = 0;
            oneOverCurveLength = 0.0f;
            fadeStart += fadeDiff;
            fadeDiff = 0.0f; // else a new mediaobject using this effect will start a 0s fade with fadeDiff != 0
            kDebug(610) << "fade ended: stay at " << fadeStart;
        }
        if (fadeStart == 0.0f) {
            memset(data + i, 0, sizeof(int16_t) *(bufferLength-i));
        } else if (fadeStart != maxVolume) {
            for (; i < bufferLength; ++i) {
                data[i] = static_cast<int16_t>(data[i] * fadeStart);
            }
        }
    /*} else if (buf->format.bits == 32) {
        float *data = reinterpret_cast<float *>(buf->mem);
        int i = 0;
        for (; curvePosition < curveLength && i < bufferLength; ++i, ++curvePosition) {
            data[i] = data[i] * curveValue(fadeStart, fadeDiff, curvePosition, oneOverCurveLength) / maxVolume;
        }
        if (curvePosition >= curveLength) {
            curveLength = 0;
            oneOverCurveLength = 0.0f;
            fadeStart = fadeStart + static_cast<int>(fadeDiff);
        }
        for (; i < bufferLength; ++i) {
            data[i] = data[i] * fadeStart / maxVolume;
        } */
    } else {
        kDebug(610) << "broken bits " << buf->format.bits;
    }
}

static void kvolumefader_port_put_buffer(xine_audio_port_t *port_gen,
        audio_buffer_t *buf, xine_stream_t *stream)
{
    post_audio_port_t *port = reinterpret_cast<post_audio_port_t *>(port_gen);
    kvolumefader_plugin_t *that = reinterpret_cast<kvolumefader_plugin_t *>(port->post);

    // modify the buffer data
    that->fadeBuffer(buf);

    // and send the modified buffer to the original port
    port->original_port->put_buffer(port->original_port, buf, stream);
    return;
}

static void kvolumefader_dispose(post_plugin_t *this_gen)
{
    kvolumefader_plugin_t *that = reinterpret_cast<kvolumefader_plugin_t *>(this_gen);

    if (_x_post_dispose(this_gen)) {
        pthread_mutex_destroy(&that->lock);
        free(that);
    }
}

/* plugin class functions */
static post_plugin_t *kvolumefader_open_plugin(post_class_t *class_gen, int inputs,
                                          xine_audio_port_t **audio_target,
                                          xine_video_port_t **video_target)
{
    Q_UNUSED(class_gen);
    Q_UNUSED(inputs);
    Q_UNUSED(video_target);

    kvolumefader_plugin_t *that = static_cast<kvolumefader_plugin_t *>(xine_xmalloc(sizeof(kvolumefader_plugin_t)));
    post_in_t             *input;
    post_out_t            *output;
    xine_post_in_t        *input_api;
    post_audio_port_t     *port;

    // refuse to work without an audio port to decorate
    if (!that || !audio_target || !audio_target[0]) {
        free(that);
        return NULL;
    }

    // creates 1 audio I/O, 0 video I/O
    _x_post_init(&that->post, 1, 0);
    pthread_mutex_init (&that->lock, NULL);

    // init private data
    that->fadeCurve = Phonon::VolumeFaderEffect::Fade3Decibel;
    that->curveValue = curveValueFadeIn3dB;
    that->fadeStart = 1.0f;
    that->fadeDiff = 0.0f;
    that->fadeTime = 0;
    that->curvePosition = 0;
    that->curveLength = 0;
    that->oneOverCurveLength = 0.0f;

    // the following call wires our plugin in front of the given audio_target
    port = _x_post_intercept_audio_port(&that->post, audio_target[0], &input, &output);
    // the methods of new_port are all forwarded to audio_target, overwrite a few of them here:
    port->new_port.open       = kvolumefader_port_open;
    port->new_port.close      = kvolumefader_port_close;
    port->new_port.put_buffer = kvolumefader_port_put_buffer;

    // add a parameter input to the plugin
    input_api       = &that->params_input;
    input_api->name = "parameters";
    input_api->type = XINE_POST_DATA_PARAMETERS;
    input_api->data = &post_api;
    xine_list_push_back(that->post.input, input_api);

    that->post.xine_post.audio_input[0] = &port->new_port;

    // our own cleanup function
    that->post.dispose = kvolumefader_dispose;

    return &that->post;
}

#if XINE_MAJOR_VERSION < 1 || ( XINE_MAJOR_VERSION == 1 && ( XINE_MINOR_VERSION < 1 || ( XINE_MINOR_VERSION == 1 && XINE_SUB_VERSION < 90 ) ) )
#define NEED_DESCRIPTION_FUNCTION 1
#else
#define NEED_DESCRIPTION_FUNCTION 0
#endif

#define PLUGIN_DESCRIPTION I18N_NOOP("Fade in or fade out with different fade curves")
#define PLUGIN_IDENTIFIER "KVolumeFader"

#if NEED_DESCRIPTION_FUNCTION
static char *kvolumefader_get_identifier(post_class_t *class_gen)
{
    Q_UNUSED(class_gen);
    return PLUGIN_IDENTIFIER;
}

static char *kvolumefader_get_description(post_class_t *class_gen)
{
    Q_UNUSED(class_gen);
    static QByteArray description(
            i18n(PLUGIN_DESCRIPTION).toUtf8());
    return description.data();
}
#endif

static void kvolumefader_class_dispose(post_class_t *class_gen)
{
    free(class_gen);
}

/* plugin class initialization function */
void *init_kvolumefader_plugin (xine_t *xine, void *)
{
    kvolumefader_class_t *_class = static_cast<kvolumefader_class_t *>(malloc(sizeof(kvolumefader_class_t)));

    if (!_class) {
        return NULL;
    }

    _class->post_class.open_plugin     = kvolumefader_open_plugin;
#if NEED_DESCRIPTION_FUNCTION
    _class->post_class.get_identifier  = kvolumefader_get_identifier;
    _class->post_class.get_description = kvolumefader_get_description;
#else
    _class->post_class.description     = PLUGIN_DESCRIPTION;
    _class->post_class.text_domain     = "phonon-xine";
    _class->post_class.identifier      = PLUGIN_IDENTIFIER;
#endif
    _class->post_class.dispose         = kvolumefader_class_dispose;

    _class->xine                       = xine;

    return _class;
}

} // extern "C"
