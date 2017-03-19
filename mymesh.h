
#ifndef MYMESH_H
#define MYMESH_H

#include <Qt3DExtras/qt3dextras_global.h>
#include <Qt3DRender/qgeometryrenderer.h>
#include <QSize>


class MyMesh : public Qt3DRender::QGeometryRenderer
{
    Q_OBJECT
    Q_PROPERTY(float width READ width WRITE setWidth NOTIFY widthChanged)
    Q_PROPERTY(float height READ height WRITE setHeight NOTIFY heightChanged)
    Q_PROPERTY(QSize meshResolution READ meshResolution WRITE setMeshResolution NOTIFY meshResolutionChanged)

public:
    explicit MyMesh(Qt3DCore::QNode *parent = nullptr);
    ~MyMesh();

    float width() const;
    float height() const;
    QSize meshResolution() const;

public Q_SLOTS:
    void setWidth(float width);
    void setHeight(float height);
    void setMeshResolution(const QSize &resolution);

Q_SIGNALS:
    void meshResolutionChanged(const QSize &meshResolution);
    void widthChanged(float width);
    void heightChanged(float height);

private:
    // As this is a default provided geometry renderer, no one should be able
    // to modify the QGeometryRenderer's properties

    void setInstanceCount(int instanceCount);
    void setVertexCount(int vertexCount);
    void setIndexOffset(int indexOffset);
    void setFirstInstance(int firstInstance);
    void setRestartIndexValue(int index);
    void setPrimitiveRestartEnabled(bool enabled);
    void setGeometry(Qt3DRender::QGeometry *geometry);
    void setPrimitiveType(PrimitiveType primitiveType);

};


#endif
