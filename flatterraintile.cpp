#include "flatterraintile.h"

#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DRender/QTexture>

#include "maptextureimage.h"
#include "quadtree.h"

class FlatTerrainTileMesh : public Qt3DRender::QGeometryRenderer
{
public:
  explicit FlatTerrainTileMesh(Qt3DExtras::QPlaneGeometry* g, Qt3DCore::QNode *parent = nullptr)
    : Qt3DRender::QGeometryRenderer(parent)
  {
    setGeometry(g);  // does not take ownership - geometry is already owned by FlatTerrain entity
  }
};


FlatTerrainTile::FlatTerrainTile(QuadTreeNode *node, Qt3DExtras::QPlaneGeometry *tileGeometry, MapTextureGenerator *mapGen, Qt3DCore::QNode *parent)
  : Qt3DCore::QEntity(parent)
{
  mesh = new FlatTerrainTileMesh(tileGeometry);
  addComponent(mesh);  // takes ownership if the component has no parent

  QgsRectangle extent = node->extent;

  material = new Qt3DExtras::QDiffuseMapMaterial;
  Qt3DRender::QTexture2D* texture = new Qt3DRender::QTexture2D(this);
  MapTextureImage* image = new MapTextureImage(mapGen, node->x, node->y, node->level);
  texture->addTextureImage(image);
  texture->setMinificationFilter(Qt3DRender::QTexture2D::Linear);
  texture->setMagnificationFilter(Qt3DRender::QTexture2D::Linear);
  material->setDiffuse(texture);
  material->setAmbient(Qt::white);
  material->setShininess(0);
  addComponent(material);  // takes ownership if the component has no parent

  transform = new Qt3DCore::QTransform();
  transform->setScale(extent.width());
  transform->setTranslation(QVector3D(extent.xMinimum() + extent.width()/2,0, -extent.yMinimum() - extent.height()/2));
  addComponent(transform);
}
