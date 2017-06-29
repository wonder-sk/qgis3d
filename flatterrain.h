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
  // TODO: virtual methods

  //!

protected:
  //! Camera used as a basis for what parts of terrain should be rendered
  Qt3DRender::QCamera *mCamera;
};

struct QuadTreeNode;

#include <Qt3DExtras/QPlaneGeometry>

class MapTextureGenerator;
class QgsRectangle;

//! generates terrain with constant height
class FlatTerrain : public AbstractTerrain
{
  Q_OBJECT
public:
  explicit FlatTerrain(MapTextureGenerator* mapGen, const QgsRectangle& extent);

  ~FlatTerrain();

  void setCamera( Qt3DRender::QCamera *camera );


private slots:
  void cameraViewMatrixChanged();

private:
  QuadTreeNode* root;
  MapTextureGenerator* mapGen;
  QList<QuadTreeNode*> activeNodes;
  Qt3DExtras::QPlaneGeometry* tileGeometry;
};

#endif // FLATTERRAIN_H
