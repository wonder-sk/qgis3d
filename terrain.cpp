#include "terrain.h"

#include "qgsrectangle.h"

#include <Qt3DExtras/QPlaneMesh>
#include <Qt3DExtras/QPlaneGeometry>

#include "map3d.h"
#include "maptextureimage.h"
#include "quadtree.h"
#include "terraintile.h"
#include "terrainboundsentity.h"



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
  root = new QuadTreeNode(extent, 0, 0, nullptr);

  // simple quad geometry shared by all tiles
  // QPlaneGeometry by default is 1x1 with mesh resultion QSize(2,2), centered at 0
  tileGeometry = new Qt3DExtras::QPlaneGeometry(this);

  // entity for drawing bounds of tiles

  ensureTileExists(root);

  bboxesEntity = new TerrainBoundsEntity(this);
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


void Terrain::addActiveNodes(QuadTreeNode* node, QList<QuadTreeNode*>& activeNodes, const QVector3D& cameraPos)
{
  ensureTileExists(node);
  float dist = node->tile->bbox.distanceFromPoint(cameraPos);

  bool willBeUsed = node->tile->minDistance <= dist || node->level == maxLevel;

  qDebug() << "node " << node->x << " " << node->y << " " << node->level << "  | extent " << node->extent.toString(1) << " --> dist " << dist  << " min dist " << node->tile->minDistance << (willBeUsed ? " -- YES " : "");

  if (willBeUsed)
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
      addActiveNodes(node->children[i], activeNodes, cameraPos);
  }
}


void Terrain::ensureTileExists(QuadTreeNode *n)
{
  if (!n->tile)
  {
    if (map.terrainType == Map3D::Flat)
      n->tile = new FlatTerrainTile(tileGeometry, n, map, this);
    else if (map.terrainType == Map3D::Dem)
      n->tile = new DemTerrainTile(n, map, this);
    else
      n->tile = new QuantizedMeshTerrainTile(n, map, this);

    n->tile->setEnabled(false); // not shown by default
  }
}


void Terrain::cameraViewMatrixChanged()
{
  qDebug() << "terrain update";

  // TODO: delete cached tiles when they have not been used for a while

  // remove all active nodes from the scene
  Q_FOREACH (QuadTreeNode* n, activeNodes)
  {
    //n->tile->setParent((Qt3DCore::QNode*)nullptr);  // crashes if we add/remove tiles like this :-(
    n->tile->setEnabled(false);
  }

  activeNodes.clear();

  addActiveNodes(root, activeNodes, mCamera->position());

  QList<AABB> bboxes;

  // add active nodes to the scene
  Q_FOREACH (QuadTreeNode* n, activeNodes)
  {
    //n->tile->setParent(this);  // crashes if we add/remove tiles like this :-(
    n->tile->setEnabled(true);
    bboxes << n->tile->bbox;
  }

  bboxesEntity->setBoxes(bboxes);

  qDebug() << "active nodes " << activeNodes.count();
}
