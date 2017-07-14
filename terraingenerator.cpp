#include "terraingenerator.h"

#include <Qt3DRender/QTexture>

#include "qgscoordinatetransform.h"

#include "map3d.h"
#include "maptexturegenerator.h"
#include "maptextureimage.h"
#include "quadtree.h"
#include "quantizedmeshterraingenerator.h"
#include "terrain.h"


TerrainTileEntity::TerrainTileEntity(Terrain* terrain, QuadTreeNode* node, Qt3DCore::QNode *parent)
  : Qt3DCore::QEntity(parent)
  , mTerrain(terrain)
  , m_textureReady(false)
{
  const Map3D& map = terrain->map3D();
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
  QgsRectangle extentMapCrs = terrain->terrainToMapTransform().transformBoundingBox(extentTerrainCrs);
  QString tileDebugText = map.drawTerrainTileInfo ? QString("%1 | %2 | %3").arg(tx).arg(ty).arg(tz) : QString();

  Qt3DRender::QTexture2D* texture = new Qt3DRender::QTexture2D(this);
  MapTextureImage* image = new MapTextureImage(terrain->mapTextureGenerator(), extentMapCrs, tileDebugText);
  connect(image, &MapTextureImage::textureReady, this, &TerrainTileEntity::onTextureReady);
  texture->addTextureImage(image);
  texture->setMinificationFilter(Qt3DRender::QTexture2D::Linear);
  texture->setMagnificationFilter(Qt3DRender::QTexture2D::Linear);
#if QT_VERSION >= 0x050900
  material = new Qt3DExtras::QTextureMaterial;
  material->setTexture(texture);
#else
  material = new Qt3DExtras::QDiffuseMapMaterial;
  material->setDiffuse(texture);
  material->setShininess(1);
  material->setAmbient(Qt::white);
#endif
  addComponent(material);  // takes ownership if the component has no parent

  // create transform for derived classes
  transform = new Qt3DCore::QTransform();
  addComponent(transform);
}

void TerrainTileEntity::onTextureReady()
{
  m_textureReady = true;
  emit textureReady();
}

// -------------------

QString TerrainGenerator::typeToString(TerrainGenerator::Type type)
{
  switch (type)
  {
    case TerrainGenerator::Flat:
      return "flat";
    case TerrainGenerator::Dem:
      return "dem";
    case TerrainGenerator::QuantizedMesh:
      return "quantized-mesh";
  }
  return QString();
}
