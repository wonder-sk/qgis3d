#include "terrain.h"

#include "qgsrectangle.h"

#include <Qt3DExtras/QPlaneMesh>
#include <Qt3DExtras/QPlaneGeometry>

#include "map3d.h"
#include "maptextureimage.h"
#include "quadtree.h"
#include "terraintile.h"



static QColor tileColors[] = {
  Qt::green,
  Qt::red,
  Qt::blue,
  Qt::cyan,
  Qt::magenta,
  Qt::yellow,
};
static int tileColorsCount = sizeof(tileColors) / sizeof(QColor);




Terrain::Terrain(Map3D& map, const QgsRectangle& extent)
  : maxLevel(0)
  , root(nullptr)
  , map(map)
{
  float minDist = extent.width();  // slightly ad-hoc min. distance - based on size of the tile 0
  if (map.terrainType == Map3D::QuantizedMesh)
    minDist = 5000;  // even more ad-hoc

  root = new QuadTreeNode(extent, 0, 0, minDist, nullptr);

  // simple quad geometry shared by all tiles
  // QPlaneGeometry by default is 1x1 with mesh resultion QSize(2,2), centered at 0
  tileGeometry = new Qt3DExtras::QPlaneGeometry(this);
}

Terrain::~Terrain()
{
  delete root;
}

void Terrain::setCamera(Qt3DRender::QCamera *camera)
{
  mCamera = camera;
  connect(mCamera, &Qt3DRender::QCamera::viewMatrixChanged, this, &Terrain::cameraViewMatrixChanged);
  cameraViewMatrixChanged();  // initial update
}


static void addActiveNodes(QuadTreeNode* node, QList<QuadTreeNode*>& activeNodes, int maxLevel, const QVector3D& cameraPos, const QgsPointXY& originOffset, const QgsCoordinateTransform& ctTerrainToMap)
{
  float dist = node->distance(cameraPos, originOffset, ctTerrainToMap);

  //qDebug() << "node " << node->x << " " << node->y << " " << node->level << "  | extent " << node->extent.toString(1) << " --> dist " << dist  << " min dist " << node->minDistance;

  if (node->minDistance <= dist || node->level == maxLevel)
  {
    // perfect fit
    activeNodes << node;
  }
  else
  {
    // needs to use children
    if (!node->children[0])
      node->makeChildren();

    for (int i = 0; i < 4; ++i)
      addActiveNodes(node->children[i], activeNodes, maxLevel, cameraPos, originOffset, ctTerrainToMap);
  }
}


void Terrain::cameraViewMatrixChanged()
{
  QVector3D cameraPos = mCamera->position();
  QgsPointXY originOffset(map.originX, map.originY);
  float dist = root->distance(cameraPos, originOffset, map.ctTerrainToMap);
  qDebug() << "camera view matrix changed " << dist;

  // TODO: ideally do not walk through the whole quadtree, only update the nodes that are being used?

  // TODO: delete cached tiles when they have not been used for a while

  // remove all active nodes from the scene
  Q_FOREACH (QuadTreeNode* n, activeNodes)
  {
    //n->tile->setParent((Qt3DCore::QNode*)nullptr);  // crashes if we add/remove tiles like this :-(
    n->tile->setEnabled(false);
  }

  activeNodes.clear();

  addActiveNodes(root, activeNodes, maxLevel, cameraPos, originOffset, map.ctTerrainToMap);

  // add active nodes to the scene
  Q_FOREACH (QuadTreeNode* n, activeNodes)
  {
    if (!n->tile)
    {
      if (map.terrainType == Map3D::Flat)
        n->tile = new FlatTerrainTile(tileGeometry, n, map, this);
      else if (map.terrainType == Map3D::Dem)
        n->tile = new DemTerrainTile(n, map, this);
      else
        n->tile = new QuantizedMeshTerrainTile(n, map, this);
    }
    //n->tile->setParent(this);  // crashes if we add/remove tiles like this :-(
    n->tile->setEnabled(true);
  }
  qDebug() << "active nodes " << activeNodes.count();
}
