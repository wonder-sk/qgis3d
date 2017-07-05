#include "terraingenerator.h"

#include <Qt3DRender/QTexture>

#include "map3d.h"
#include "maptexturegenerator.h"
#include "maptextureimage.h"
#include "quadtree.h"
#include "quantizedmeshterraingenerator.h"


TerrainTileEntity::TerrainTileEntity(QuadTreeNode* node, const Map3D& map, Qt3DCore::QNode *parent)
  : Qt3DCore::QEntity(parent)
{
  int tx, ty, tz;
  if (map.terrainGenerator->type() == TerrainGenerator::QuantizedMesh)
  {
    // TODO: sort out - should not be here
    QuantizedMeshTerrainGenerator* generator = static_cast<QuantizedMeshTerrainGenerator*>(map.terrainGenerator.get());
    generator->quadTreeTileToBaseTile(node->x, node->y, node->level, tx, ty, tz);
  }
  else
  {
    tx = node->x;
    ty = node->y;
    tz = node->level;
  }

  QgsRectangle extentTerrainCrs = map.terrainGenerator->terrainTilingScheme.tileToExtent(tx, ty, tz);
  QgsRectangle extentMapCrs = map.ctTerrainToMap.transformBoundingBox(extentTerrainCrs);
  QString tileDebugText = QString("%1 | %2 | %3").arg(tx).arg(ty).arg(tz);

  material = new Qt3DExtras::QTextureMaterial;
  Qt3DRender::QTexture2D* texture = new Qt3DRender::QTexture2D(this);
  MapTextureImage* image = new MapTextureImage(map.mapGen, extentMapCrs, tileDebugText);
  texture->addTextureImage(image);
  texture->setMinificationFilter(Qt3DRender::QTexture2D::Linear);
  texture->setMagnificationFilter(Qt3DRender::QTexture2D::Linear);
  material->setTexture(texture);
  addComponent(material);  // takes ownership if the component has no parent

  // create transform for derived classes
  transform = new Qt3DCore::QTransform();
  addComponent(transform);
}

// -------------------
