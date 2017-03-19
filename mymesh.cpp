
#include "mymesh.h"
#include "mygeometry.h"

#include <QImage>

MyMesh::MyMesh(QNode *parent)
    : QGeometryRenderer(parent)
{

    MyGeometry *geometry = new MyGeometry(this);
    QObject::connect(geometry, &MyGeometry::widthChanged, this, &MyMesh::widthChanged);
    QObject::connect(geometry, &MyGeometry::heightChanged, this, &MyMesh::heightChanged);
    QObject::connect(geometry, &MyGeometry::resolutionChanged, this, &MyMesh::meshResolutionChanged);
    QGeometryRenderer::setGeometry(geometry);
}

MyMesh::~MyMesh()
{
}

void MyMesh::setWidth(float width)
{
    static_cast<MyGeometry *>(geometry())->setWidth(width);
}

float MyMesh::width() const
{
    return static_cast<MyGeometry *>(geometry())->width();
}

void MyMesh::setHeight(float height)
{
    static_cast<MyGeometry *>(geometry())->setHeight(height);
}

float MyMesh::height() const
{
    return static_cast<MyGeometry *>(geometry())->height();
}

void MyMesh::setMeshResolution(const QSize &resolution)
{
    static_cast<MyGeometry *>(geometry())->setResolution(resolution);
}

QSize MyMesh::meshResolution() const
{
    return static_cast<MyGeometry *>(geometry())->resolution();
}
