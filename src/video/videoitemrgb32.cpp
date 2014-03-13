#include "videoitem.h"

#include <QSGGeometryNode>
#include <QSGSimpleRectNode>


#include <QOpenGLFunctions>

#include <QMutexLocker>


#include "videosurfaceinterface.h"
#include "frontend_p.h"
#include "factory_p.h"
#include "abstractvideooutput_p.h"
#include <QImage>

namespace Phonon {



class VideoMaterialShader : public QSGMaterialShader
{
public:
    VideoMaterialShader()
        : QSGMaterialShader(),
          m_id_matrix(-1),
          m_id_rgbTexture(-1),
          m_id_opacity(-1)
//          m_pixelFormat(pixelFormat)
    {
    }

    void updateState(const RenderState &state, QSGMaterial *newMaterial, QSGMaterial *oldMaterial);

    virtual char const *const *attributeNames() const Q_DECL_OVERRIDE
    {
        qDebug() << Q_FUNC_INFO;
        static const char *names[] = {
            "qt_VertexPosition",
            "qt_VertexTexCoord",
            0
        };
        return names;
    }

protected:

    virtual const char *vertexShader() const Q_DECL_OVERRIDE
    {
        qDebug() << Q_FUNC_INFO;
        const char *shader =
                "uniform highp mat4 qt_Matrix;                      \n"
                "attribute highp vec4 qt_VertexPosition;            \n"
                "attribute highp vec2 qt_VertexTexCoord;            \n"
                "varying highp vec2 qt_TexCoord;                    \n"
                "void main() {                                      \n"
                "    qt_TexCoord = qt_VertexTexCoord;               \n"
                "    gl_Position = qt_Matrix * qt_VertexPosition;   \n"
                "}";
        return shader;
    }

    virtual const char *fragmentShader() const Q_DECL_OVERRIDE
    {
            qDebug() << Q_FUNC_INFO;
        static const char *shader =
                "uniform sampler2D rgbTexture;"
                "uniform lowp float opacity;"
                ""
                "varying highp vec2 qt_TexCoord;"
                ""
                "void main()"
                "{"
                "    gl_FragColor =  vec4(texture2D(rgbTexture, qt_TexCoord).bgr, 1.0) * opacity;"
                "}";
        return shader;
    }

    virtual void initialize() Q_DECL_OVERRIDE
    {
        qDebug() << Q_FUNC_INFO;
        m_id_matrix = program()->uniformLocation("qt_Matrix");
        m_id_rgbTexture = program()->uniformLocation("rgbTexture");
        m_id_opacity = program()->uniformLocation("opacity");
    }

private:
    int m_id_matrix;
    int m_id_rgbTexture;
    int m_id_opacity;
};




class VideoMaterial : public QSGMaterial
{
public:
    VideoMaterial(VideoSurfaceOutputInterface *interface)
        : m_textureId(0)
        , m_opacity(1.0)
        , m_interface(interface)
    {
        qDebug() << Q_FUNC_INFO;
        setFlag(Blending, false);
    }

#warning dafuq
    virtual QSGMaterialType *type() const Q_DECL_OVERRIDE
    {
        static QSGMaterialType theType;
        return &theType;
    }

    virtual QSGMaterialShader *createShader() const Q_DECL_OVERRIDE
    {
        return new VideoMaterialShader;
    }

    virtual int compare(const QSGMaterial *other) const Q_DECL_OVERRIDE
    {
        const VideoMaterial *m = static_cast<const VideoMaterial *>(other);
        return m_textureId - m->m_textureId;
    }

    void updateBlending()
    {
        setFlag(Blending, qFuzzyCompare(m_opacity, qreal(1.0)) ? false : true);
    }

    void bind();
    void bindTexture(int id, int w, int h, const char *bits, int pitch);

#warning shader accesses shit
//private:

    QMutex m_frameMutex;
    QSize m_textureSize;
    GLuint m_textureId;
    qreal m_opacity;

    VideoSurfaceOutputInterface *m_interface;
};



void VideoMaterialShader::updateState(const RenderState &state,
                                          QSGMaterial *newMaterial,
                                          QSGMaterial *oldMaterial)
{
    qDebug() << Q_FUNC_INFO;
    Q_UNUSED(oldMaterial);

    VideoMaterial *mat = static_cast<VideoMaterial *>(newMaterial);
    program()->setUniformValue(m_id_rgbTexture, 0);

    mat->bind();

    if (state.isOpacityDirty()) {
        mat->m_opacity = state.opacity();
        mat->updateBlending();
        program()->setUniformValue(m_id_opacity, GLfloat(mat->m_opacity));
    }

    if (state.isMatrixDirty())
        program()->setUniformValue(m_id_matrix, state.combinedMatrix());
}




void VideoMaterial::bind()
{
    qDebug() << Q_FUNC_INFO;
    QOpenGLFunctions *functions = QOpenGLContext::currentContext()->functions();

    Q_ASSERT(m_interface);

    m_interface->lock();
    if (m_interface->frame()->isValid()) {
        qDebug() << "valid";
//        if (m_frame.map(QAbstractVideoBuffer::ReadOnly)) {

            // Frame has changed size, recreate textures...
            if (m_textureSize != m_interface->frame()->size()) {
                if (!m_textureSize.isEmpty())
                    glDeleteTextures(1, &m_textureId);
                glGenTextures(1, &m_textureId);
                m_textureSize = m_interface->frame()->size();
            }


//            QImage i((const uchar *)m_interface->frame()->plane[0].data(),
//                    m_interface->frame()->width,
//                    m_interface->frame()->height,
//                    QImage::Format_RGB32);
//            i.save("/home/me/testb.png");

            GLint dataType = GL_UNSIGNED_BYTE;
            GLint dataFormat = GL_RGBA;

            functions->glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, m_textureId);
            glTexImage2D(GL_TEXTURE_2D, 0, dataFormat,
                         m_textureSize.width(), m_textureSize.height(),
                         0, dataFormat, dataType, m_interface->frame()->plane[0].data());

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

//            m_interface->frame()->unmap();
//        }
#warning mooo
//        m_frame = VideoFrame();
    } else {
        functions->glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_textureId);
    }

#warning lock release here!!
    m_interface->unlock();
    qDebug() << Q_FUNC_INFO << "END";
}

void VideoMaterial::bindTexture(int id, int w, int h, const char *bits, int pitch)
{
    qDebug() << Q_FUNC_INFO;
    glBindTexture(GL_TEXTURE_2D, id);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, pitch); // weird videos with funny pitch
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, w, h, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, bits);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0); // reset to default
}



class VideoNode : public QSGGeometryNode
{
public:
    VideoNode(VideoSurfaceOutputInterface *interface)
        : QSGGeometryNode()
        , m_material(0)
    {
        setFlag(QSGNode::OwnsMaterial);
        m_material = new VideoMaterial(interface);
        setMaterial(m_material);
    }

    ~VideoNode()
    {
        delete m_material;
    }

private:
    VideoMaterial *m_material;
};




class VideoItemPrivate : public AbstractVideoOutputPrivate
{
public:
    VideoItemPrivate(VideoItem *parent)
        : AbstractVideoOutputPrivate()
        , interface(0)
//        , frame(0)
    {
        qDebug() << Q_FUNC_INFO;
        createBackendObject();
    }

    void createBackendObject() Q_DECL_OVERRIDE
    {
        qDebug() << Q_FUNC_INFO;
        m_backendObject = Factory::createVideoSurfaceOutput();
        Q_ASSERT(m_backendObject);
        interface = qobject_cast<VideoSurfaceOutputInterface *>(m_backendObject);
        Q_ASSERT(interface);
//        if (!m_backendObject || !interface) {
//            frame = new VideoFrame;
//        } else
//            frame = const_cast<VideoFrame *>(interface->frame());
//        Q_ASSERT(frame);
    }

    VideoSurfaceOutputInterface *interface;

//    VideoFrame *frame;

private:
    P_DECLARE_PUBLIC(VideoItem)
};




VideoItem::VideoItem(QQuickItem *parent)
    : QQuickItem(parent)
    , AbstractVideoOutput(*new VideoItemPrivate(this))
{
    P_D(VideoItem);
    qDebug() << Q_FUNC_INFO;
    setFlag(ItemHasContents, true);
    connect(d->backendObject(), SIGNAL(frameReady()), this, SLOT(onFrameReady()));
}

VideoItem::~VideoItem()
{
    qDebug() << Q_FUNC_INFO;
}

void VideoItem::onFrameReady()
{
    update();
}

QSGNode *VideoItem::updatePaintNode(QSGNode *node, QQuickItem::UpdatePaintNodeData *updatePaintNodeData)
{
    P_D(VideoItem);
    qDebug() << Q_FUNC_INFO;

    Q_ASSERT(d->interface);

    VideoNode *n = static_cast<VideoNode *>(node);
    if (!node)
        n = new VideoNode(d->interface);

    qDebug() << "@@#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@";
    qDebug() << n->geometry();
    qDebug() << boundingRect();
//    QSGGeometry::updateTexturedRectGeometry(n->geometry(), boundingRect(), QRectF(0, 0, 1, 1));
//    static_cast<VideoMaterial*>(n->material())->state()->color = m_color;
//    n->setRe

    qDebug() << d->interface->frame()->format;
    if (!d->interface->frame()->isValid()) {
        qDebug() << "no frame, no painty";
        return 0;
    }






    QSGGeometry *g = n->geometry();

    if (g == 0)
        g = new QSGGeometry(QSGGeometry::defaultAttributes_TexturedPoint2D(), 4);

    QSGGeometry::TexturedPoint2D *v = g->vertexDataAsTexturedPoint2D();

    // top left
    v->set(0, 0, 0, 0);
    // bottom left
    (++v)->set(0, height(), 0, 1);
    // top right
    (++v)->set(width(), 0, 1, 0);
    // bottom right
    (++v)->set(width(), height(), 1, 1);

    if (!n->geometry())
        n->setGeometry(g);
    n->markDirty(QSGNode::DirtyGeometry);



//    QImage i((const uchar *)d->interface->frame()->plane[0].data(),
//            d->interface->frame()->width,
//            d->interface->frame()->height,
//            QImage::Format_RGB32);
//    i.save("/home/me/testa.png");




#warning rect calc missing entirely

    n->markDirty(QSGNode::DirtyGeometry | QSGNode::DirtyMaterial);

    return n;

//    QSGSimpleRectNode *n = static_cast<QSGSimpleRectNode *>(oldNode);
//    if (!n) {
//        n = new QSGSimpleRectNode();
//        n->setColor(Qt::blue);
//    }
//    n->setRect(boundingRect());
//    return n;
}

} // namespace Phonon
