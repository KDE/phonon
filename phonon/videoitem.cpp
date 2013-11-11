#include "videoitem.h"

#include <QSGGeometryNode>
#include <QSGSimpleRectNode>


#include <QOpenGLFunctions>

#include <QMutexLocker>


#include "videosurfaceinterface.h"
#include "frontend_p.h"
#include "factory_p.h"
#include "abstractvideooutput_p.h"


namespace Phonon {



class VideoMaterialShader : public QSGMaterialShader
{
public:
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
        "uniform sampler2D yTexture;"
        "uniform sampler2D uTexture;"
        "uniform sampler2D vTexture;"
        "uniform mediump mat4 colorMatrix;"
        "uniform lowp float opacity;"
        ""
        "varying highp vec2 qt_TexCoord;"
        ""
        "void main()"
        "{"
        "    mediump float Y = texture2D(yTexture, qt_TexCoord).r;"
        "    mediump float U = texture2D(uTexture, qt_TexCoord).r;"
        "    mediump float V = texture2D(vTexture, qt_TexCoord).r;"
        "    mediump vec4 color = vec4(Y, U, V, 1.);"
        "    gl_FragColor = colorMatrix * color * opacity;"
        "}";
        return shader;
    }

    virtual void initialize() Q_DECL_OVERRIDE
    {
        qDebug() << Q_FUNC_INFO;
        m_id_matrix = program()->uniformLocation("qt_Matrix");
        m_id_yTexture = program()->uniformLocation("yTexture");
        m_id_uTexture = program()->uniformLocation("uTexture");
        m_id_vTexture = program()->uniformLocation("vTexture");
        m_id_colorMatrix = program()->uniformLocation("colorMatrix");
        m_id_opacity = program()->uniformLocation("opacity");
    }

private:
    int m_id_matrix;
    int m_id_yTexture;
    int m_id_uTexture;
    int m_id_vTexture;
    int m_id_colorMatrix;
    int m_id_opacity;
};




class VideoMaterial : public QSGMaterial
{
public:
    VideoMaterial(VideoSurfaceOutputInterface *interface)
        : m_opacity(1.0)
        , m_interface(interface)
    {
        qDebug() << Q_FUNC_INFO;
        memset(m_textureIds, 0, sizeof(m_textureIds));

#warning get from VGO... the following is crap
//        switch (format.yCbCrColorSpace()) {
//        case QVideoSurfaceFormat::YCbCr_JPEG:
//            m_colorMatrix = QMatrix4x4(
//                        1.0f,  0.000f,  1.402f, -0.701f,
//                        1.0f, -0.344f, -0.714f,  0.529f,
//                        1.0f,  1.772f,  0.000f, -0.886f,
//                        0.0f,  0.000f,  0.000f,  1.0000f);
//            break;
//        case QVideoSurfaceFormat::YCbCr_BT709:
//        case QVideoSurfaceFormat::YCbCr_xvYCC709:
//            m_colorMatrix = QMatrix4x4(
//                        1.164f,  0.000f,  1.793f, -0.5727f,
//                        1.164f, -0.534f, -0.213f,  0.3007f,
//                        1.164f,  2.115f,  0.000f, -1.1302f,
//                        0.0f,    0.000f,  0.000f,  1.0000f);
//            break;
//        default: //BT 601:
//            m_colorMatrix = QMatrix4x4(
//                        1.164f,  0.000f,  1.596f, -0.8708f,
//                        1.164f, -0.392f, -0.813f,  0.5296f,
//                        1.164f,  2.017f,  0.000f, -1.081f,
//                        0.0f,    0.000f,  0.000f,  1.0000f);
//        }
        m_colorMatrix = QMatrix4x4(
            1.164383561643836,  0.0000,             1.792741071428571, -0.972945075016308,
            1.164383561643836, -0.21324861427373,  -0.532909328559444,  0.301482665475862,
            1.164383561643836,  2.112401785714286,  0.0000,            -1.133402217873451,
            0.0000,             0.0000,             0.0000,             1.0000           );

        setFlag(Blending, false);
    }

    virtual QSGMaterialShader *createShader() const Q_DECL_OVERRIDE
    {
        return new VideoMaterialShader;
    }

    virtual int compare(const QSGMaterial *other) const Q_DECL_OVERRIDE
    {
        const VideoMaterial *m = static_cast<const VideoMaterial *>(other);
        int d = m_textureIds[0] - m->m_textureIds[0];
        if (d)
            return d;
        else if ((d = m_textureIds[1] - m->m_textureIds[1]) != 0)
            return d;
        else
            return m_textureIds[2] - m->m_textureIds[2];
    }

#warning dafuq
    virtual QSGMaterialType *type() const Q_DECL_OVERRIDE
    {
        static QSGMaterialType theType;
        return &theType;
    }


    void bind();
    void bindTexture(int id, int w, int h, const char *bits, int pitch);

#warning shader accesses shit
//private:
//    QVideoSurfaceFormat m_format;
    QSize m_textureSize;

    static const uint Num_Texture_IDs = 3;
    GLuint m_textureIds[Num_Texture_IDs];

    qreal m_opacity;
    QMatrix4x4 m_colorMatrix;

    VideoSurfaceOutputInterface *m_interface;
};



void VideoMaterialShader::updateState(const RenderState &state,
                                          QSGMaterial *newMaterial,
                                          QSGMaterial *oldMaterial)
{
    qDebug() << Q_FUNC_INFO;
    Q_UNUSED(oldMaterial);

    VideoMaterial *mat = static_cast<VideoMaterial *>(newMaterial);
    program()->setUniformValue(m_id_yTexture, 0);
    program()->setUniformValue(m_id_uTexture, 1);
    program()->setUniformValue(m_id_vTexture, 2);

    mat->bind();

    program()->setUniformValue(m_id_colorMatrix, mat->m_colorMatrix);
    if (state.isOpacityDirty()) {
        mat->m_opacity = state.opacity();
        program()->setUniformValue(m_id_opacity, GLfloat(mat->m_opacity));
    }

    if (state.isMatrixDirty())
        program()->setUniformValue(m_id_matrix, state.combinedMatrix());
    qDebug() << Q_FUNC_INFO<<"END";
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
                qDebug() << "RETEXXXXX";
                if (!m_textureSize.isEmpty())
                    glDeleteTextures(Num_Texture_IDs, m_textureIds);
                glGenTextures(Num_Texture_IDs, m_textureIds);
                m_textureSize = m_interface->frame()->size();
            }

            functions->glActiveTexture(GL_TEXTURE1);
            int i = 1;
            bindTexture(m_textureIds[1],
                    m_interface->frame()->visiblePitch[i],
                    m_interface->frame()->visibleLines[i],
                    m_interface->frame()->plane[i].data(),
                    m_interface->frame()->pitch[i]);
            functions->glActiveTexture(GL_TEXTURE2);
#warning two and one are inverted.....
            i = 2;
            bindTexture(m_textureIds[2],
                    m_interface->frame()->visiblePitch[i],
                    m_interface->frame()->visibleLines[i],
                    m_interface->frame()->plane[i].data(),
                    m_interface->frame()->pitch[i]);
            functions->glActiveTexture(GL_TEXTURE0); // Finish with 0 as default texture unit
            i = 0;
            bindTexture(m_textureIds[0],
                    m_interface->frame()->visiblePitch[i],
                    m_interface->frame()->visibleLines[i],
                    m_interface->frame()->plane[i].data(),
                    m_interface->frame()->pitch[i]);

//            m_interface->frame()->unmap();
//        }
#warning mooo
//        m_frame = VideoFrame();
    } else {
        functions->glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_textureIds[1]);
        functions->glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, m_textureIds[2]);
        functions->glActiveTexture(GL_TEXTURE0); // Finish with 0 as default texture unit
        glBindTexture(GL_TEXTURE_2D, m_textureIds[0]);
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
    setFlag(QQuickItem::ItemHasContents, true);
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
