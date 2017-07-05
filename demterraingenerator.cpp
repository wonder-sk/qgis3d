#include "demterraingenerator.h"

#include "map3d.h"
#include "quadtree.h"
#include "demterraintilegeometry.h"
#include "maptexturegenerator.h"

#include <Qt3DRender/QGeometryRenderer>

#include "qgsrasterlayer.h"

//! tile made from DEM
class DemTerrainTile : public TerrainTileEntity
{
public:
  DemTerrainTile(QuadTreeNode* node, const Map3D& map, Qt3DCore::QNode *parent = nullptr);

private:
};

DemTerrainTile::DemTerrainTile(QuadTreeNode *node, const Map3D& map, Qt3DCore::QNode *parent)
  : TerrainTileEntity(node, map, parent)
{
  DemTerrainGenerator* generator = static_cast<DemTerrainGenerator*>(map.terrainGenerator.get());

  // TODO: make it async
  QByteArray heightMap = generator->tGen->render(node->x, node->y, node->level);

  const float* zBits = (const float*) heightMap.constData();
  int zCount = heightMap.count() / sizeof(float);
  float zMin = zBits[0], zMax = zBits[0];
  for (int i = 0; i < zCount; ++i)
  {
    float z = zBits[i];
    zMin = qMin(zMin, z);
    zMax = qMax(zMax, z);
  }

  Qt3DRender::QGeometryRenderer* mesh = new Qt3DRender::QGeometryRenderer;
  mesh->setGeometry(new DemTerrainTileGeometry(generator->tGen->resolution(), heightMap, mesh));
  addComponent(mesh);  // takes ownership if the component has no parent

  QgsRectangle extent = node->extent;
  double x0 = extent.xMinimum() - map.originX;
  double y0 = extent.yMinimum() - map.originY;
  double side = extent.width();
  double half = side/2;

  transform->setScale3D(QVector3D(side, map.zExaggeration, side));
  transform->setTranslation(QVector3D(x0 + half,0, - (y0 + half)));

  bbox = AABB(x0, zMin*map.zExaggeration, -y0, x0 + side, zMax*map.zExaggeration, -(y0 + side));
  epsilon = side / map.tileTextureSize;  // use texel size as the error
}


// ---------------


DemTerrainGenerator::DemTerrainGenerator(QgsRasterLayer *dem, int terrainSize)
{
  demLayer = dem;
  demTerrainSize = terrainSize;
  terrainTilingScheme = TilingScheme(dem->extent(), dem->crs());
  tGen.reset(new TerrainTextureGenerator(demLayer, terrainTilingScheme, demTerrainSize));
}

TerrainGenerator::Type DemTerrainGenerator::type() const
{
  return TerrainGenerator::Dem;
}

QgsRectangle DemTerrainGenerator::extent() const
{
  return terrainTilingScheme.tileToExtent(0, 0, 0);
}

TerrainTileEntity *DemTerrainGenerator::createTile(QuadTreeNode *n, const Map3D &map, Qt3DCore::QNode *parent) const
{
  return new DemTerrainTile(n, map, parent);
}
