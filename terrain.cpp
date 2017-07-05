#include "terrain.h"

#include "map3d.h"
#include "maptextureimage.h"
#include "quadtree.h"
#include "terraingenerator.h"
#include "terrainboundsentity.h"

#include "qgsrectangle.h"


float screenSpaceError(float epsilon, float distance, float screenSize, float fov)
{
  /* This routine approximately calculates how an error (epsilon) of an object in world coordinates
   * at given distance (between camera and the object) will look like in screen coordinates.
   *
   * the math below simply uses triangle similarity:
   *
   *             epsilon                       phi
   *   -----------------------------  = ----------------
   *   [ frustum width at distance ]    [ screen width ]
   *
   * Then we solve for phi, substituting [frustum width at distance] = 2 * distance * tan(fov / 2)
   *
   *  ________xxx__      xxx = real world error (epsilon)
   *  \     |     /        x = screen space error (phi)
   *   \    |    /
   *    \___|_x_/   near plane (screen space)
   *     \  |  /
   *      \ | /
   *       \|/    angle = field of view
   *       camera
   */
  float phi = epsilon * screenSize / (2 * distance * tan( fov * M_PI / (2 * 180) ) );
  return phi;
}


#if 0
static QColor tileColors[] = {
  Qt::green,
  Qt::red,
  Qt::blue,
  Qt::cyan,
  Qt::magenta,
  Qt::yellow,
};
static int tileColorsCount = sizeof(tileColors) / sizeof(QColor);
#endif


Terrain::Terrain(Map3D& map, const QgsRectangle& extent)
  : maxLevel(0)
  , root(nullptr)
  , map(map)
  , screenSizePx(0)
{
  root = new QuadTreeNode(extent, 0, 0, nullptr);

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

void Terrain::setViewport(const QRect &rect)
{
  screenSizePx = qMax(rect.width(), rect.height());  // TODO: is this correct?
  cameraViewMatrixChanged();  // update terrain tiles accordingly
}


void Terrain::addActiveNodes(QuadTreeNode* node, QList<QuadTreeNode*>& activeNodes, const QVector3D& cameraPos, float cameraFov)
{
  ensureTileExists(node);
  float dist = node->tile->bbox.distanceFromPoint(cameraPos);

  float sse = screenSpaceError(node->tile->epsilon, dist, screenSizePx, cameraFov);

  float tau = 1.0;  // max. allowed screen space error

  float tileMinDistance = node->tile->epsilon * screenSizePx / (2 * tau * tan(cameraFov * M_PI / (180*2)));

  bool willBeUsed = sse <= tau || node->level == maxLevel;

  qDebug() << "node " << node->x << " " << node->y << " " << node->level << "  | extent " << node->extent.toString(1) << " --> dist " << dist  << " min dist " << tileMinDistance << (willBeUsed ? " -- YES " : "") << "   sse " << sse;

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
      addActiveNodes(node->children[i], activeNodes, cameraPos, cameraFov);
  }
}


void Terrain::ensureTileExists(QuadTreeNode *n)
{
  if (!n->tile)
  {
    n->tile = map.terrainGenerator->createTile(n, map, this);
    n->tile->setEnabled(false); // not shown by default
  }
}


void Terrain::cameraViewMatrixChanged()
{
  if (!mCamera || !screenSizePx)
    return;  // not yet fully initialized

  qDebug() << "terrain update";

  // TODO: delete cached tiles when they have not been used for a while

  // remove all active nodes from the scene
  Q_FOREACH (QuadTreeNode* n, activeNodes)
  {
    //n->tile->setParent((Qt3DCore::QNode*)nullptr);  // crashes if we add/remove tiles like this :-(
    n->tile->setEnabled(false);
  }

  activeNodes.clear();

  addActiveNodes(root, activeNodes, mCamera->position(), mCamera->fieldOfView());

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
