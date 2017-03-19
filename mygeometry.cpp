
#include "mygeometry.h"
#include <Qt3DRender/qattribute.h>
#include <Qt3DRender/qbuffer.h>
#include <Qt3DRender/qbufferdatagenerator.h>
#include <limits>


using namespace Qt3DRender;


QByteArray createPlaneVertexData(float w, float h, const QSize &resolution, const QImage& i)
{
    Q_ASSERT(w > 0.0f);
    Q_ASSERT(h > 0.0f);
    Q_ASSERT(resolution.width() >= 2);
    Q_ASSERT(resolution.height() >= 2);

    const uchar* zBits = i.bits();
    int z_width = i.width(), z_height = i.height();
    int z_step_w = z_width / resolution.width(), z_step_h = z_height / resolution.height();
    double z_scale = 1/15.;

    const int nVerts = resolution.width() * resolution.height();

    // Populate a buffer with the interleaved per-vertex data with
    // vec3 pos, vec2 texCoord, vec3 normal, vec4 tangent
    const quint32 elementSize = 3 + 2 + 3 + 4;
    const quint32 stride = elementSize * sizeof(float);
    QByteArray bufferBytes;
    bufferBytes.resize(stride * nVerts);
    float* fptr = reinterpret_cast<float*>(bufferBytes.data());

    const float x0 = -w / 2.0f;
    const float z0 = -h / 2.0f;
    const float dx = w / (resolution.width() - 1);
    const float dz = h / (resolution.height() - 1);
    const float du = 1.0 / (resolution.width() - 1);
    const float dv = 1.0 / (resolution.height() - 1);

    // Iterate over z
    for (int j = 0; j < resolution.height(); ++j) {
        const float z = z0 + static_cast<float>(j) * dz;
        const float v = static_cast<float>(j) * dv;

        // Iterate over x
        for (int i = 0; i < resolution.width(); ++i) {
            const float x = x0 + static_cast<float>(i) * dx;
            const float u = static_cast<float>(i) * du;

            // position
            *fptr++ = x;
            *fptr++ = zBits[i*z_step_w + (z_height-1-j*z_step_h)*z_width] * z_scale;
            *fptr++ = z;

            // texture coordinates
            *fptr++ = u;
            *fptr++ = v;

            // normal
            *fptr++ = 0.0f;
            *fptr++ = 1.0f;
            *fptr++ = 0.0f;

            // tangent
            *fptr++ = 1.0f;
            *fptr++ = 0.0f;
            *fptr++ = 0.0f;
            *fptr++ = 1.0f;
        }
    }

    return bufferBytes;
}

QByteArray createPlaneIndexData(const QSize &resolution)
{
    // Create the index data. 2 triangles per rectangular face
    const int faces = 2 * (resolution.width() - 1) * (resolution.height() - 1);
    const int indices = 3 * faces;
    Q_ASSERT(indices < std::numeric_limits<quint32>::max());
    QByteArray indexBytes;
    indexBytes.resize(indices * sizeof(quint32));
    quint32* indexPtr = reinterpret_cast<quint32*>(indexBytes.data());

    // Iterate over z
    for (int j = 0; j < resolution.height() - 1; ++j) {
        const int rowStartIndex = j * resolution.width();
        const int nextRowStartIndex = (j + 1) * resolution.width();

        // Iterate over x
        for (int i = 0; i < resolution.width() - 1; ++i) {
            // Split quad into two triangles
            *indexPtr++ = rowStartIndex + i;
            *indexPtr++ = nextRowStartIndex + i;
            *indexPtr++ = rowStartIndex + i + 1;

            *indexPtr++ = nextRowStartIndex + i;
            *indexPtr++ = nextRowStartIndex + i + 1;
            *indexPtr++ = rowStartIndex + i + 1;
        }
    }

    return indexBytes;
}


class PlaneVertexBufferFunctor : public QBufferDataGenerator
{
public:
    explicit PlaneVertexBufferFunctor(float w, float h, const QSize &resolution, const QImage& i)
        : m_width(w)
        , m_height(h)
        , m_resolution(resolution)
        , m_i(i)
    {}

    ~PlaneVertexBufferFunctor() {}

    QByteArray operator()() Q_DECL_FINAL
    {
        return createPlaneVertexData(m_width, m_height, m_resolution, m_i);
    }

    bool operator ==(const QBufferDataGenerator &other) const Q_DECL_FINAL
    {
        const PlaneVertexBufferFunctor *otherFunctor = functor_cast<PlaneVertexBufferFunctor>(&other);
        if (otherFunctor != nullptr)
            return (otherFunctor->m_width == m_width &&
                    otherFunctor->m_height == m_height &&
                    otherFunctor->m_resolution == m_resolution);
        return false;
    }

    QT3D_FUNCTOR(PlaneVertexBufferFunctor)

    private:
        float m_width;
    float m_height;
    QSize m_resolution;
    QImage m_i;
};

class PlaneIndexBufferFunctor : public QBufferDataGenerator
{
public:
    explicit PlaneIndexBufferFunctor(const QSize &resolution)
        : m_resolution(resolution)
    {}

    ~PlaneIndexBufferFunctor() {}

    QByteArray operator()() Q_DECL_FINAL
    {
        return createPlaneIndexData(m_resolution);
    }

    bool operator ==(const QBufferDataGenerator &other) const Q_DECL_FINAL
    {
        const PlaneIndexBufferFunctor *otherFunctor = functor_cast<PlaneIndexBufferFunctor>(&other);
        if (otherFunctor != nullptr)
            return (otherFunctor->m_resolution == m_resolution);
        return false;
    }

    QT3D_FUNCTOR(PlaneIndexBufferFunctor)

    private:
        QSize m_resolution;
};




MyGeometry::MyGeometry(MyGeometry::QNode *parent)
    : QGeometry(parent)
    , m_width(1.0f)
    , m_height(1.0f)
    , m_meshResolution(QSize(2, 2))
    , m_positionAttribute(nullptr)
    , m_normalAttribute(nullptr)
    , m_texCoordAttribute(nullptr)
    , m_tangentAttribute(nullptr)
    , m_indexAttribute(nullptr)
    , m_vertexBuffer(nullptr)
    , m_indexBuffer(nullptr)
{
    init();
}

MyGeometry::~MyGeometry()
{
}


void MyGeometry::updateVertices()
{
    const int nVerts = m_meshResolution.width() * m_meshResolution.height();

    m_positionAttribute->setCount(nVerts);
    m_normalAttribute->setCount(nVerts);
    m_texCoordAttribute->setCount(nVerts);
    m_tangentAttribute->setCount(nVerts);
    m_vertexBuffer->setDataGenerator(QSharedPointer<PlaneVertexBufferFunctor>::create(m_width, m_height, m_meshResolution, i));
}


void MyGeometry::updateIndices()
{
    const int faces = 2 * (m_meshResolution.width() - 1) * (m_meshResolution.height() - 1);
    // Each primitive has 3 vertices
    m_indexAttribute->setCount(faces * 3);
    m_indexBuffer->setDataGenerator(QSharedPointer<PlaneIndexBufferFunctor>::create(m_meshResolution));
}


void MyGeometry::setResolution(const QSize &resolution)
{
    if (m_meshResolution == resolution)
        return;
    m_meshResolution = resolution;
    updateVertices();
    updateIndices();
    emit resolutionChanged(resolution);
}

void MyGeometry::setWidth(float width)
{
    if (width == m_width)
        return;
    m_width = width;
    updateVertices();
    emit widthChanged(width);
}

void MyGeometry::setHeight(float height)
{
    if (height == m_height)
        return;
    m_height = height;
    updateVertices();
    emit heightChanged(height);
}


QSize MyGeometry::resolution() const
{
    return m_meshResolution;
}


float MyGeometry::width() const
{
    return m_width;
}


float MyGeometry::height() const
{
    return m_height;
}

QAttribute *MyGeometry::positionAttribute() const
{
    return m_positionAttribute;
}

QAttribute *MyGeometry::normalAttribute() const
{
    return m_normalAttribute;
}

QAttribute *MyGeometry::texCoordAttribute() const
{
    return m_texCoordAttribute;
}

QAttribute *MyGeometry::tangentAttribute() const
{
    return m_tangentAttribute;
}

QAttribute *MyGeometry::indexAttribute() const
{
    return m_indexAttribute;
}


void MyGeometry::init()
{
  // load img with DTM
  bool res = i.load(":/dtm.tif");
  Q_ASSERT(res);
  qDebug("w %d h %d", i.width(), i.height() );
  qDebug("format %d", i.format());
  Q_ASSERT(i.width() == 1800 && i.height() == 1800);
  Q_ASSERT(i.format() == 24); // greyscale


    m_positionAttribute = new QAttribute(this);
    m_normalAttribute = new QAttribute(this);
    m_texCoordAttribute = new QAttribute(this);
    m_tangentAttribute = new QAttribute(this);
    m_indexAttribute = new QAttribute(this);
    m_vertexBuffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer, this);
    m_indexBuffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::IndexBuffer, this);

    const int nVerts = m_meshResolution.width() * m_meshResolution.height();
    const int stride = (3 + 2 + 3 + 4) * sizeof(float);
    const int faces = 2 * (m_meshResolution.width() - 1) * (m_meshResolution.height() - 1);

    m_positionAttribute->setName(QAttribute::defaultPositionAttributeName());
    m_positionAttribute->setVertexBaseType(QAttribute::Float);
    m_positionAttribute->setVertexSize(3);
    m_positionAttribute->setAttributeType(QAttribute::VertexAttribute);
    m_positionAttribute->setBuffer(m_vertexBuffer);
    m_positionAttribute->setByteStride(stride);
    m_positionAttribute->setCount(nVerts);

    m_texCoordAttribute->setName(QAttribute::defaultTextureCoordinateAttributeName());
    m_texCoordAttribute->setVertexBaseType(QAttribute::Float);
    m_texCoordAttribute->setVertexSize(2);
    m_texCoordAttribute->setAttributeType(QAttribute::VertexAttribute);
    m_texCoordAttribute->setBuffer(m_vertexBuffer);
    m_texCoordAttribute->setByteStride(stride);
    m_texCoordAttribute->setByteOffset(3 * sizeof(float));
    m_texCoordAttribute->setCount(nVerts);

    m_normalAttribute->setName(QAttribute::defaultNormalAttributeName());
    m_normalAttribute->setVertexBaseType(QAttribute::Float);
    m_normalAttribute->setVertexSize(3);
    m_normalAttribute->setAttributeType(QAttribute::VertexAttribute);
    m_normalAttribute->setBuffer(m_vertexBuffer);
    m_normalAttribute->setByteStride(stride);
    m_normalAttribute->setByteOffset(5 * sizeof(float));
    m_normalAttribute->setCount(nVerts);

    m_tangentAttribute->setName(QAttribute::defaultTangentAttributeName());
    m_tangentAttribute->setVertexBaseType(QAttribute::Float);
    m_tangentAttribute->setVertexSize(4);
    m_tangentAttribute->setAttributeType(QAttribute::VertexAttribute);
    m_tangentAttribute->setBuffer(m_vertexBuffer);
    m_tangentAttribute->setByteStride(stride);
    m_tangentAttribute->setByteOffset(8 * sizeof(float));
    m_tangentAttribute->setCount(nVerts);

    m_indexAttribute->setAttributeType(QAttribute::IndexAttribute);
    m_indexAttribute->setVertexBaseType(QAttribute::UnsignedInt);
    m_indexAttribute->setBuffer(m_indexBuffer);

    // Each primitive has 3 vertives
    m_indexAttribute->setCount(faces * 3);

    m_vertexBuffer->setDataGenerator(QSharedPointer<PlaneVertexBufferFunctor>::create(m_width, m_height, m_meshResolution, i));
    m_indexBuffer->setDataGenerator(QSharedPointer<PlaneIndexBufferFunctor>::create(m_meshResolution));

    addAttribute(m_positionAttribute);
    addAttribute(m_texCoordAttribute);
    addAttribute(m_normalAttribute);
    addAttribute(m_tangentAttribute);
    addAttribute(m_indexAttribute);
}
