#include "quadtree.h"

#include "terraingenerator.h"

#include "qgscoordinatetransform.h"

QuadTreeNode::QuadTreeNode(const QgsRectangle &e, int tileX, int tileY, QuadTreeNode *par)
  : extent(e), tile(nullptr), parent(par)
{
  for (int i = 0; i < 4; ++i)
    children[i] = nullptr;

  x = tileX;
  y = tileY;

  if (parent)
  {
    level = par->level + 1;
  }
  else
  {
    level = 0;
  }
}

QuadTreeNode::~QuadTreeNode()
{
  for (int i = 0; i < 4; ++i)
    delete children[i];
  delete tile;
}


void QuadTreeNode::makeChildren()
{
  Q_ASSERT(!children[0]);
  double xmin = extent.xMinimum(), xmax = extent.xMaximum();
  double ymin = extent.yMinimum(), ymax = extent.yMaximum();
  double xc = (xmin+xmax)/2, yc = (ymin+ymax)/2;
  //  2 | 3
  // ---+---
  //  0 | 1

  int baseTileX = x * 2;
  int baseTileY = y * 2;

  children[0] = new QuadTreeNode(QgsRectangle(xmin,ymin,xc,yc), baseTileX, baseTileY, this);
  children[1] = new QuadTreeNode(QgsRectangle(xc,ymin,xmax,yc), baseTileX+1, baseTileY, this);
  children[2] = new QuadTreeNode(QgsRectangle(xmin,yc,xc,ymax), baseTileX, baseTileY+1, this);
  children[3] = new QuadTreeNode(QgsRectangle(xc,yc,xmax,ymax), baseTileX+1, baseTileY+1, this);
}

float QuadTreeNode::distance(const QVector3D &pos, const QgsPointXY& originOffset, const QgsCoordinateTransform& ctTerrainToMap)
{
  QgsPointXY c = ctTerrainToMap.transform(extent.center());
  QVector3D tileCenter(c.x()-originOffset.x(), 0, -c.y()+originOffset.y());
  return tileCenter.distanceToPoint(pos);
}
