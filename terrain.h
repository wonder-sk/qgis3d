#ifndef FLATTERRAIN_H
#define FLATTERRAIN_H

#include <Qt3DCore/QEntity>
#include <Qt3DRender/QCamera>

//! base class for all implementations of terrain.
//! on top of terrain there is always map image as a texture.
//! derived classes should internally do tiling
class AbstractTerrain : public Qt3DCore::QEntity
{
public:
  AbstractTerrain() : mCamera(nullptr) {}
  // TODO: virtual methods

  //!

protected:
  //! Camera used as a basis for what parts of terrain should be rendered
  Qt3DRender::QCamera *mCamera;
};

class Map3D;
struct QuadTreeNode;
class TerrainBoundsEntity;

#include <Qt3DExtras/QPlaneGeometry>

class QgsRectangle;

//! generates terrain with constant height
class Terrain : public AbstractTerrain
{
  Q_OBJECT
public:
  explicit Terrain(Map3D& map, const QgsRectangle& extent);

  ~Terrain();

  void setCamera( Qt3DRender::QCamera *camera );
  void setViewport( const QRect& rect );

  void setMaxLevel( int level ) { maxLevel = level; }


private slots:
  void cameraViewMatrixChanged();

private:
  void addActiveNodes(QuadTreeNode* node, QList<QuadTreeNode*>& activeNodes, const QVector3D& cameraPos, float cameraFov);
  void ensureTileExists(QuadTreeNode* node);

private:
  int maxLevel;
  QuadTreeNode* root;
  Map3D& map;
  QList<QuadTreeNode*> activeNodes;
  Qt3DExtras::QPlaneGeometry* tileGeometry;

  TerrainBoundsEntity* bboxesEntity;
  int screenSizePx;
};

#endif // FLATTERRAIN_H
