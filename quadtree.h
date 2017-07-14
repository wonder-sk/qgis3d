#ifndef QUADTREE_H
#define QUADTREE_H

#include "qgsrectangle.h"

class TerrainTileEntity;
class QgsCoordinateTransform;

//! Quad tree data structure to keep track of terrain tiles
class QuadTreeNode
{
public:
  explicit QuadTreeNode(const QgsRectangle& e, int tileX, int tileY, QuadTreeNode* par);

  QuadTreeNode(const QuadTreeNode& other) = delete;

  ~QuadTreeNode();

  void makeChildren();

  float distance(const QVector3D& pos, const QgsPointXY& originOffset, const QgsCoordinateTransform& ctTerrainToMap);

  QgsRectangle extent;  //!< extent in terrain's CRS
  TerrainTileEntity *tile; //!< null = not yet created

  int level;   //!< how deep are we in the tree (root = level 0)
  int x,y;     //!< coordinates of the tile in the current level. (0,0) is bottom-left tile.
  QuadTreeNode* parent;
  QuadTreeNode* children[4];
};

#endif // QUADTREE_H
