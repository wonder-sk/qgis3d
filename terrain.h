#ifndef TERRAIN_H
#define TERRAIN_H

#include <Qt3DCore/QEntity>

namespace Qt3DRender
{
  class QCamera;
}

class Map3D;
struct QuadTreeNode;
class TerrainBoundsEntity;

class QgsRectangle;

/**
 * Controller for terrain - decides on what terrain tiles to show based on camera position
 * and creates them using map's terrain tile generator.
 */
class Terrain : public Qt3DCore::QEntity
{
  Q_OBJECT
public:
  explicit Terrain(const Map3D& map);

  ~Terrain();

  void setCamera( Qt3DRender::QCamera *camera );
  void setViewport( const QRect& rect );

  void setMaxLevel( int level ) { maxLevel = level; }


public slots:
  void cameraViewMatrixChanged();

private:
  void addActiveNodes(QuadTreeNode* node, QList<QuadTreeNode*>& activeNodes, const QVector3D& cameraPos, float cameraFov);
  void ensureTileExists(QuadTreeNode* node);

private:
  //! Camera used as a basis for what parts of terrain should be rendered
  Qt3DRender::QCamera *mCamera;

  int maxLevel;
  QuadTreeNode* root;
  const Map3D& map;
  QList<QuadTreeNode*> activeNodes;

  TerrainBoundsEntity* bboxesEntity;
  int screenSizePx;
};

#endif // TERRAIN_H
