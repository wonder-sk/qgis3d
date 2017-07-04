#include "terraintile.h"

#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DRender/QTexture>

#include "map3d.h"
#include "maptexturegenerator.h"
#include "maptextureimage.h"
#include "quadtree.h"
#include "terraintilegeometry.h"


TerrainTile::TerrainTile(QuadTreeNode* node, Map3D& map, Qt3DCore::QNode *parent)
  : Qt3DCore::QEntity(parent)
{
  int tx, ty, tz;
  if (map.terrainType == Map3D::QuantizedMesh)
  {
    QuantizedMeshTerrainTile::tileExtentXYZ(node, map, tx, ty, tz);
  }
  else
  {
    tx = node->x;
    ty = node->y;
    tz = node->level;
  }

  QgsRectangle extentTerrainCrs = map.terrainTilingScheme.tileToExtent(tx, ty, tz);
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


// ----------------


FlatTerrainTile::FlatTerrainTile(Qt3DExtras::QPlaneGeometry *tileGeometry, QuadTreeNode *node, Map3D& map, Qt3DCore::QNode *parent)
  : TerrainTile(node, map, parent)
{
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
  minDistance = side;  // slightly ad-hoc min. distance
}


// ----------------


DemTerrainTile::DemTerrainTile(QuadTreeNode *node, Map3D& map, Qt3DCore::QNode *parent)
  : TerrainTile(node, map, parent)
{
  // TODO: make it async
  QByteArray heightMap = map.tGen->render(node->x, node->y, node->level);

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
  mesh->setGeometry(new TerrainTileGeometry(map.tGen->resolution(), heightMap, mesh));
  addComponent(mesh);  // takes ownership if the component has no parent

  QgsRectangle extent = node->extent;
  double x0 = extent.xMinimum() - map.originX;
  double y0 = extent.yMinimum() - map.originY;
  double side = extent.width();
  double half = side/2;

  transform->setScale3D(QVector3D(side, map.zExaggeration, side));
  transform->setTranslation(QVector3D(x0 + half,0, - (y0 + half)));

  bbox = AABB(x0, zMin*map.zExaggeration, -y0, x0 + side, zMax*map.zExaggeration, -(y0 + side));
  minDistance = side;  // slightly ad-hoc min. distance
}


// ----------------

#include "quantizedmeshgeometry.h"
#include "qgsmapsettings.h"

QuantizedMeshTerrainTile::QuantizedMeshTerrainTile(QuadTreeNode *node, Map3D& map, Qt3DCore::QNode *parent)
  : TerrainTile(node, map, parent)
{
  int tx, ty, tz;
  tileExtentXYZ(node, map, tx, ty, tz);

  QgsRectangle tileRect = map.terrainTilingScheme.tileToExtent(tx, ty, tz);

  // we need map settings here for access to mapToPixel
  QgsMapSettings mapSettings;
  mapSettings.setLayers(map.layers);
  mapSettings.setOutputSize(QSize(map.tileTextureSize,map.tileTextureSize));
  mapSettings.setDestinationCrs(map.crs);
  mapSettings.setExtent(map.ctTerrainToMap.transformBoundingBox(tileRect));

  QuantizedMeshGeometry::downloadTileIfMissing(tx, ty, tz);
  QuantizedMeshTile* qmt = QuantizedMeshGeometry::readTile(tx, ty, tz, tileRect);
  Q_ASSERT(qmt);
  Qt3DRender::QGeometryRenderer* mesh = new Qt3DRender::QGeometryRenderer;
  mesh->setGeometry(new QuantizedMeshGeometry(qmt, map, mapSettings.mapToPixel(), mesh));
  addComponent(mesh);

  transform->setScale3D(QVector3D(1.f, map.zExaggeration, 1.f));

  QgsRectangle mapExtent = mapSettings.extent();
  float x0 = mapExtent.xMinimum(), y0 = mapExtent.yMinimum();
  float x1 = mapExtent.xMaximum(), y1 = mapExtent.yMaximum();
  float z0 = qmt->header.MinimumHeight, z1 = qmt->header.MaximumHeight;
  bbox = AABB(x0, z0*map.zExaggeration, -y0, x1, z1*map.zExaggeration, -y1);
  minDistance = 5000 / pow(2, node->level);  // even more ad-hoc
}

void QuantizedMeshTerrainTile::tileExtentXYZ(QuadTreeNode *node, Map3D &map, int &tx, int &ty, int &tz)
{
  // true tile coords (using the base tile pos)
  int multiplier = pow(2, node->level);
  tx = map.terrainBaseX * multiplier + node->x;
  ty = map.terrainBaseY * multiplier + node->y;
  tz = map.terrainBaseZ + node->level;
}
