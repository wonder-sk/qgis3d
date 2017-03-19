
#ifndef MYGEOMETRY_H
#define MYGEOMETRY_H

#include <Qt3DExtras/qt3dextras_global.h>
#include <Qt3DRender/qgeometry.h>
#include <QSize>

#include <QImage>

namespace Qt3DRender {

class QAttribute;
class QBuffer;

} // Qt3DRender


class MyGeometry : public Qt3DRender::QGeometry
{
    Q_OBJECT
    Q_PROPERTY(float width READ width WRITE setWidth NOTIFY widthChanged)
    Q_PROPERTY(float height READ height WRITE setHeight NOTIFY heightChanged)
    Q_PROPERTY(QSize resolution READ resolution WRITE setResolution NOTIFY resolutionChanged)
    Q_PROPERTY(Qt3DRender::QAttribute *positionAttribute READ positionAttribute CONSTANT)
    Q_PROPERTY(Qt3DRender::QAttribute *normalAttribute READ normalAttribute CONSTANT)
    Q_PROPERTY(Qt3DRender::QAttribute *texCoordAttribute READ texCoordAttribute CONSTANT)
    Q_PROPERTY(Qt3DRender::QAttribute *tangentAttribute READ tangentAttribute CONSTANT)
    Q_PROPERTY(Qt3DRender::QAttribute *indexAttribute READ indexAttribute CONSTANT)

public:
    explicit MyGeometry(QNode *parent = nullptr);
    ~MyGeometry();

    void updateVertices();
    void updateIndices();

    QSize resolution() const;
    float width() const;
    float height() const;

    Qt3DRender::QAttribute *positionAttribute() const;
    Qt3DRender::QAttribute *normalAttribute() const;
    Qt3DRender::QAttribute *texCoordAttribute() const;
    Qt3DRender::QAttribute *tangentAttribute() const;
    Qt3DRender::QAttribute *indexAttribute() const;

public Q_SLOTS:
    void setResolution(const QSize &resolution);
    void setWidth(float width);
    void setHeight(float height);

Q_SIGNALS:
    void resolutionChanged(const QSize &resolution);
    void widthChanged(float width);
    void heightChanged(float height);

protected:
    //QPlaneGeometry(QPlaneGeometryPrivate &dd, QNode *parent = nullptr);

private:
    void init();

    QImage i;

    float m_width;
    float m_height;
    QSize m_meshResolution;
    Qt3DRender::QAttribute *m_positionAttribute;
    Qt3DRender::QAttribute *m_normalAttribute;
    Qt3DRender::QAttribute *m_texCoordAttribute;
    Qt3DRender::QAttribute *m_tangentAttribute;
    Qt3DRender::QAttribute *m_indexAttribute;
    Qt3DRender::QBuffer *m_vertexBuffer;
    Qt3DRender::QBuffer *m_indexBuffer;
};


#endif
