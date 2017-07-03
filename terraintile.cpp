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


class FlatTerrainTileMesh : public Qt3DRender::QGeometryRenderer
{
public:
  explicit FlatTerrainTileMesh(Qt3DExtras::QPlaneGeometry* g, Qt3DCore::QNode *parent = nullptr)
    : Qt3DRender::QGeometryRenderer(parent)
  {
    setGeometry(g);  // does not take ownership - geometry is already owned by FlatTerrain entity
  }
};


FlatTerrainTile::FlatTerrainTile(Qt3DExtras::QPlaneGeometry *tileGeometry, QuadTreeNode *node, Map3D& map, Qt3DCore::QNode *parent)
  : TerrainTile(node, map, parent)
{
  mesh = new FlatTerrainTileMesh(tileGeometry);
  addComponent(mesh);  // takes ownership if the component has no parent

  // set up transform according to the extent covered by the quad geometry
  QgsRectangle extent = node->extent;

  double x0 = extent.xMinimum() - map.originX;
  double y0 = extent.yMinimum() - map.originY;
  double side = extent.width();
  double half = side/2;

  transform->setScale(extent.width());
  //transform->setTranslation(QVector3D(extent.width()/2 + extent.width()*node->x,0, - extent.height()/2 - extent.height() * node->y));
  //transform->setTranslation(QVector3D(extent.xMinimum() + extent.width()/2,0, -extent.yMinimum() - extent.height()/2));
  transform->setTranslation(QVector3D(x0 + half,0, - (y0 + half)));
}


// ----------------


DemTerrainTile::DemTerrainTile(QuadTreeNode *node, Map3D& map, Qt3DCore::QNode *parent)
  : TerrainTile(node, map, parent)
{
  // TODO: make it async
  QByteArray heightMap = map.tGen->render(node->x, node->y, node->level);

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
}

void QuantizedMeshTerrainTile::tileExtentXYZ(QuadTreeNode *node, Map3D &map, int &tx, int &ty, int &tz)
{
  // true tile coords (using the base tile pos)
  int multiplier = pow(2, node->level);
  tx = map.terrainBaseX * multiplier + node->x;
  ty = map.terrainBaseY * multiplier + node->y;
  tz = map.terrainBaseZ + node->level;
}
