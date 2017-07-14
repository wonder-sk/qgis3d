#include "terrain.h"

#include "map3d.h"
#include "maptextureimage.h"
#include "maptexturegenerator.h"
#include "quadtree.h"
#include "terraingenerator.h"
#include "terrainboundsentity.h"

#include "qgsrectangle.h"

#include <Qt3DRender/QCamera>


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


Terrain::Terrain(const Map3D& map)
  : mCamera(nullptr)
  , maxLevel(0)
  , root(nullptr)
  , map(map)
  , bboxesEntity(nullptr)
  , screenSizePx(0)
{
  QgsRectangle extent = map.terrainGenerator->extent();

  root = new QuadTreeNode(extent, 0, 0, nullptr);

  mTerrainToMapTransform = new QgsCoordinateTransform(map.terrainGenerator->crs(), map.crs);

  mMapTextureGenerator = new MapTextureGenerator(map);

  // entity for drawing bounds of tiles
  ensureTileExists(root);

  if (map.showBoundingBoxes)
    bboxesEntity = new TerrainBoundsEntity(this);
}

Terrain::~Terrain()
{
  delete root;
  delete mMapTextureGenerator;
  delete mTerrainToMapTransform;
}

void Terrain::setCamera(Qt3DRender::QCamera *camera)
{
  mCamera = camera;
  cameraViewMatrixChanged();  // initial update

  // we do not listen directly to camera's matrix changed signals because we would update
  // terrain too often (a camera update is made of four partial updates).
  // The window owning terrain sends signals when camera controller changes camera position.
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

  float tau = 2.0;  // max. allowed screen space error

  bool willBeUsed = sse <= tau || node->level == maxLevel;

  //float tileMinDistance = node->tile->epsilon * screenSizePx / (2 * tau * tan(cameraFov * M_PI / (180*2)));
  //qDebug() << "node " << node->x << " " << node->y << " " << node->level << "  | extent " << node->extent.toString(1) << /*" --> dist " << dist  << " min dist " << tileMinDistance << (willBeUsed ? " -- YES " : "") <<*/ "   sse " << sse;

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

    // test whether all children are ready to replace this parent tile.
    // if not, we still need to display the parent tile, otherwise children could only
    // display some placeholder texture
    for (int i = 0; i < 4; ++i)
    {
      QuadTreeNode* childNode = node->children[i];
      ensureTileExists(childNode);
      if (!childNode->tile->m_textureReady)
      {
        activeNodes << node;
        //qDebug() << "skipping children: not ready!";
        return;
      }
    }

    // child tiles are ready - we can use them
    for (int i = 0; i < 4; ++i)
      addActiveNodes(node->children[i], activeNodes, cameraPos, cameraFov);
  }
}


void Terrain::ensureTileExists(QuadTreeNode *n)
{
  if (!n->tile)
  {
    n->tile = map.terrainGenerator->createTile(this, n, this);
    n->tile->setEnabled(false); // not shown by default
    // force update of terrain
    connect(n->tile, &TerrainTileEntity::textureReady, this, &Terrain::cameraViewMatrixChanged);
  }
}


void Terrain::cameraViewMatrixChanged()
{
  if (!mCamera || !screenSizePx)
    return;  // not yet fully initialized

  qDebug() << "terrain update";

  // TODO: delete cached tiles when they have not been used for a while
#if 0
  QSet<QuadTreeNode*> oldActiveNodes = QSet<QuadTreeNode*>::fromList(activeNodes);
  // ... update tiles here ...
  QSet<QuadTreeNode*> newActiveNodes = QSet<QuadTreeNode*>::fromList(activeNodes);
  Q_FOREACH (QuadTreeNode* n, oldActiveNodes)
  {
    if (!newActiveNodes.contains(n))
    {
      n->tile->deleteLater();
      n->tile = nullptr;
    }
  }
#endif

  // remove all active nodes from the scene
  Q_FOREACH (QuadTreeNode* n, activeNodes)
  {
    n->tile->setEnabled(false);
  }

  activeNodes.clear();

  addActiveNodes(root, activeNodes, mCamera->position(), mCamera->fieldOfView());

  QList<AABB> bboxes;

  // add active nodes to the scene
  Q_FOREACH (QuadTreeNode* n, activeNodes)
  {
    n->tile->setEnabled(true);
    bboxes << n->tile->bbox;
  }

  if (bboxesEntity)
    bboxesEntity->setBoxes(bboxes);

  qDebug() << "active nodes " << activeNodes.count();
}
