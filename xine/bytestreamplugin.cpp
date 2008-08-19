/*  This file is part of the KDE project
    Copyright (C) 2006 Matthias Kretz <kretz@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2
    as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
    02110-1301, USA.

*/

#include <kdemacros.h>

#include <QExplicitlySharedDataPointer>

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include <xine.h>
#include "bytestream.h"
#include "myshareddatapointer.h"

extern "C" {
// xine headers use the reserved keyword this:
#define this this_xine
#include <xine/compat.h>
#include <xine/input_plugin.h>
#include <xine/xine_internal.h>
#include <xine/xineutils.h>
#include "net_buf_ctrl.h"
#undef this

static void kbytestream_pause_cb(void *that_gen);
static void kbytestream_normal_cb(void *that_gen);
class KByteStreamInputPlugin : public input_plugin_t
{
public:
    KByteStreamInputPlugin(xine_stream_t *stream, const char *_mrl);

    ~KByteStreamInputPlugin();

    inline const char *mrl() const { return m_mrl.constData(); }

    inline xine_stream_t *stream() { return m_stream; }

    inline Phonon::Xine::ByteStream *bytestream() { return m_bytestream.data(); }

private:
    xine_stream_t *m_stream;
    nbc_t *m_nbc;
    const QByteArray m_mrl;
    MySharedDataPointer<Phonon::Xine::ByteStream> m_bytestream;
};

KByteStreamInputPlugin::KByteStreamInputPlugin(xine_stream_t *stream, const char *_mrl)
    : m_stream(stream),
    m_nbc(nbc_init(stream)),
    m_mrl(_mrl),
    m_bytestream(Phonon::Xine::ByteStream::fromMrl(m_mrl))
{
    if (!m_bytestream) {
        return;
    }
    input_plugin_t *that = this;
    memset(that, 0, sizeof(input_plugin_t));

    m_bytestream->ref.ref();


    nbc_set_pause_cb(m_nbc, kbytestream_pause_cb, this);
    nbc_set_normal_cb(m_nbc, kbytestream_normal_cb, this);
}

KByteStreamInputPlugin::~KByteStreamInputPlugin()
{
    if (m_nbc) {
        nbc_close(m_nbc);
    }
    if (m_bytestream && !m_bytestream->ref.deref()) {
        m_bytestream->deleteLater();
    }
}

static uint32_t kbytestream_plugin_get_capabilities (input_plugin_t *this_gen)
{
    KByteStreamInputPlugin *that = static_cast<KByteStreamInputPlugin *>(this_gen);
    return INPUT_CAP_PREVIEW | (that->bytestream()->streamSeekable() ? INPUT_CAP_SEEKABLE : 0);
}

/* remove the !__APPLE__ junk once the osx xine stuff is merged back into xine 1.2 proper */
#if ((XINE_SUB_VERSION >= 90 && XINE_MINOR_VERSION == 1 && !defined __APPLE__) || (XINE_MINOR_VERSION > 1) && XINE_MAJOR_VERSION == 1) || XINE_MAJOR_VERSION > 1
static off_t kbytestream_plugin_read (input_plugin_t *this_gen, void *buf, off_t len)
#else
static off_t kbytestream_plugin_read (input_plugin_t *this_gen, char *buf, off_t len)
#endif
{
    KByteStreamInputPlugin *that = static_cast<KByteStreamInputPlugin *>(this_gen);
    off_t read = that->bytestream()->readFromBuffer(buf, len);
    return read;
}

static buf_element_t *kbytestream_plugin_read_block (input_plugin_t *this_gen, fifo_buffer_t *fifo, off_t todo)
{
    KByteStreamInputPlugin *that = static_cast<KByteStreamInputPlugin *>(this_gen);
    buf_element_t *buf = fifo->buffer_pool_alloc(fifo);

    buf->content = buf->mem;
    buf->type = BUF_DEMUX_BLOCK;
    buf->size = 0;

    while (buf->size < todo) {
        const off_t num_bytes = that->bytestream()->readFromBuffer(buf->mem + buf->size, todo - buf->size);
        if (num_bytes <= 0) {
            buf->free_buffer(buf);
            return NULL;
        }
        buf->size += num_bytes;
    }

    return buf;
}

static off_t kbytestream_plugin_seek (input_plugin_t *this_gen, off_t offset, int origin)
{
    KByteStreamInputPlugin *that = static_cast<KByteStreamInputPlugin *>(this_gen);
    switch (origin) {
    case SEEK_SET:
        break;
    case SEEK_CUR:
        offset += that->bytestream()->currentPosition();
        break;
    case SEEK_END:
        offset += that->bytestream()->streamSize();
        break;
    }

    return that->bytestream()->seekBuffer(offset);
}

static off_t kbytestream_plugin_get_current_pos (input_plugin_t *this_gen)
{
    KByteStreamInputPlugin *that = static_cast<KByteStreamInputPlugin *>(this_gen);
    return that->bytestream()->currentPosition();
}

static off_t kbytestream_plugin_get_length (input_plugin_t *this_gen)
{
    KByteStreamInputPlugin *that = static_cast<KByteStreamInputPlugin *>(this_gen);
    return that->bytestream()->streamSize();
}

static uint32_t kbytestream_plugin_get_blocksize (input_plugin_t *)
{
    return 0;
}

#if (XINE_SUB_VERSION > 3 && XINE_MINOR_VERSION == 1) || (XINE_MINOR_VERSION > 1 && XINE_MAJOR_VERSION == 1) || XINE_MAJOR_VERSION > 1
static const char *kbytestream_plugin_get_mrl (input_plugin_t *this_gen)
{
    KByteStreamInputPlugin *that = static_cast<KByteStreamInputPlugin *>(this_gen);
    return that->mrl();
}
#else
static char *kbytestream_plugin_get_mrl (input_plugin_t *this_gen)
{
    KByteStreamInputPlugin *that = static_cast<KByteStreamInputPlugin *>(this_gen);
    return const_cast<char *>(that->mrl());
}
#endif

static int kbytestream_plugin_get_optional_data (input_plugin_t *this_gen,
        void *data, int data_type)
{
    if (data_type == INPUT_OPTIONAL_DATA_PREVIEW) {
        KByteStreamInputPlugin *that = static_cast<KByteStreamInputPlugin *>(this_gen);
        return that->bytestream()->peekBuffer(data);
    }
    return INPUT_OPTIONAL_UNSUPPORTED;
}

static void kbytestream_plugin_dispose (input_plugin_t *this_gen)
{
    KByteStreamInputPlugin *that = static_cast<KByteStreamInputPlugin *>(this_gen);
    delete that;
}

static int kbytestream_plugin_open (input_plugin_t *this_gen)
{
    kDebug(610);
    KByteStreamInputPlugin *that = static_cast<KByteStreamInputPlugin *>(this_gen);

    if (kbytestream_plugin_get_length (this_gen) == 0) {
        _x_message(that->stream(), XINE_MSG_FILE_EMPTY, that->mrl(), NULL);
        xine_log (that->stream()->xine, XINE_LOG_MSG,
                "input_kbytestream: File empty: >%s<\n", that->mrl());
        return 0;
    }

    Q_ASSERT(that->bytestream());
    that->bytestream()->reset();

    return 1;
}

static void kbytestream_pause_cb(void *that_gen)
{
    KByteStreamInputPlugin *that = static_cast<KByteStreamInputPlugin *>(that_gen);
    that->bytestream()->setPauseForBuffering(true);
}

static void kbytestream_normal_cb(void *that_gen)
{
    KByteStreamInputPlugin *that = static_cast<KByteStreamInputPlugin *>(that_gen);
    that->bytestream()->setPauseForBuffering(false);
}

static input_plugin_t *kbytestream_class_get_instance (input_class_t *cls_gen, xine_stream_t *stream,
        const char *mrl)
{
    kDebug(610);
    KByteStreamInputPlugin *that = new KByteStreamInputPlugin(stream, mrl);

    if (!that->bytestream()) {
        delete that;
        return NULL;
    }

    that->open               = kbytestream_plugin_open;
    that->get_capabilities   = kbytestream_plugin_get_capabilities;
    that->read               = kbytestream_plugin_read;
    that->read_block         = kbytestream_plugin_read_block;
    that->seek               = kbytestream_plugin_seek;
    that->get_current_pos    = kbytestream_plugin_get_current_pos;
    that->get_length         = kbytestream_plugin_get_length;
    that->get_blocksize      = kbytestream_plugin_get_blocksize;
    that->get_mrl            = kbytestream_plugin_get_mrl;
    that->get_optional_data  = kbytestream_plugin_get_optional_data;
    that->dispose            = kbytestream_plugin_dispose;
    that->input_class        = cls_gen;

    return that;
}

#define PLUGIN_DESCRIPTION "kbytestream input plugin"
#define PLUGIN_IDENTIFIER "kbytestream"

#if XINE_MAJOR_VERSION < 1 || ( XINE_MAJOR_VERSION == 1 && ( XINE_MINOR_VERSION < 1 || ( XINE_MINOR_VERSION == 1 && XINE_SUB_VERSION < 90 ) ) )
#define NEED_DESCRIPTION_FUNCTION 1
#else
#define NEED_DESCRIPTION_FUNCTION 0
#endif

#if NEED_DESCRIPTION_FUNCTION
#if (XINE_SUB_VERSION > 3 && XINE_MINOR_VERSION == 1) || (XINE_MINOR_VERSION > 1 && XINE_MAJOR_VERSION == 1) || XINE_MAJOR_VERSION > 1
static const char *kbytestream_class_get_description(input_class_t *)
{
    return PLUGIN_DESCRIPTION;
}
#else
static char *kbytestream_class_get_description(input_class_t *)
{
    return const_cast<char *>(PLUGIN_DESCRIPTION);
}
#endif

static const char *kbytestream_class_get_identifier(input_class_t *)
{
    return PLUGIN_IDENTIFIER;
}
#endif /* NEED_DESCRIPTION_FUNCTIONS */

static void kbytestream_class_dispose (input_class_t *this_gen)
{
    delete this_gen;
}

void *init_kbytestream_plugin (xine_t *xine, void *data)
{
    Q_UNUSED(xine);
    Q_UNUSED(data);
    input_class_t *that = new input_class_t;
    memset(that, 0, sizeof(that));

    that->get_instance       = kbytestream_class_get_instance;
#if NEED_DESCRIPTION_FUNCTION
    that->get_identifier     = kbytestream_class_get_identifier;
    that->get_description    = kbytestream_class_get_description;
#else
    that->description        = PLUGIN_DESCRIPTION;
    that->text_domain        = "phonon-xine";
    that->identifier         = PLUGIN_IDENTIFIER;
#endif
    that->get_dir            = NULL;
    that->get_autoplay_list  = NULL;
    that->dispose            = kbytestream_class_dispose;
    that->eject_media        = NULL;

    return that;
}
} // extern "C"

/* vim: sw=4 et
 */
