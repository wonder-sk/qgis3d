#include "flatterraingenerator.h"

#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DExtras/QPlaneGeometry>

#include "map3d.h"
#include "quadtree.h"
#include "terrain.h"

//! just a simple quad with a map texture
class FlatTerrainTile : public TerrainTileEntity
{
public:
  FlatTerrainTile(Qt3DExtras::QPlaneGeometry* tileGeometry, Terrain* terrain, QuadTreeNode* node, Qt3DCore::QNode *parent = nullptr);

private:
};

FlatTerrainTile::FlatTerrainTile(Qt3DExtras::QPlaneGeometry *tileGeometry, Terrain* terrain, QuadTreeNode *node, Qt3DCore::QNode *parent)
  : TerrainTileEntity(terrain, node, parent)
{
  const Map3D& map = terrain->map3D();
  Qt3DRender::QGeometryRenderer* mesh = new Qt3DRender::QGeometryRenderer;
  mesh->setGeometry(tileGeometry);  // does not take ownership - geometry is already owned by FlatTerrain entity
  addComponent(mesh);  // takes ownership if the component has no parent

  // set up transform according to the extent covered by the quad geometry
  QgsRectangle extent = node->extent;

  double x0 = extent.xMinimum() - map.originX;
  double y0 = extent.yMinimum() - map.originY;
  double side = extent.width();
  double half = side/2;

  transform->setScale(extent.width());
  transform->setTranslation(QVector3D(x0 + half,0, - (y0 + half)));

  bbox = AABB(x0, 0, -y0, x0 + side, 0, -(y0 + side));
  epsilon = side / map.tileTextureSize;  // no geometric error - use texel size as the error
}


// ---------------


FlatTerrainGenerator::FlatTerrainGenerator()
{
  // simple quad geometry shared by all tiles
  // QPlaneGeometry by default is 1x1 with mesh resultion QSize(2,2), centered at 0
  tileGeometry = new Qt3DExtras::QPlaneGeometry;  // TODO: parent to a node...
}

TerrainGenerator::Type FlatTerrainGenerator::type() const
{
  return TerrainGenerator::Flat;
}

QgsRectangle FlatTerrainGenerator::extent() const
{
  return terrainTilingScheme.tileToExtent(0, 0, 0);
}

TerrainTileEntity *FlatTerrainGenerator::createTile(Terrain* terrain, QuadTreeNode *n, Qt3DCore::QNode *parent) const
{
  return new FlatTerrainTile(tileGeometry, terrain, n, parent);
}

void FlatTerrainGenerator::setExtent(const QgsRectangle &extent, const QgsCoordinateReferenceSystem &crs)
{
  // the real extent will be a square where the given extent fully fits
  terrainTilingScheme = TilingScheme(extent, crs);
}
