#ifndef QUADTREE_H
#define QUADTREE_H

#include "qgsrectangle.h"

class TerrainTile;
class MapTextureGenerator;

#include <Qt3DExtras/QPlaneGeometry>


//! Quad tree data structure to keep track of terrain tiles
struct QuadTreeNode
{
  explicit QuadTreeNode(const QgsRectangle& e, int tileX, int tileY, QuadTreeNode* par);

  QuadTreeNode(const QuadTreeNode& other) = delete;

  ~QuadTreeNode();

  void makeChildren();

  float distance(const QVector3D& pos);

  QgsRectangle extent;
  TerrainTile *tile; //!< null = not yet created
  float minDistance;  //!< if camera is closer than this distance, we need to switch to children

  int level;   //!< how deep are we in the tree (root = level 0)
  int x,y;     //!< coordinates of the tile in the current level. (0,0) is bottom-left tile.
  QuadTreeNode* parent;
  QuadTreeNode* children[4];
};

#endif // QUADTREE_H
