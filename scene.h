#ifndef SCENE_H
#define SCENE_H

#include <Qt3DCore/QEntity>

namespace Qt3DRender
{
  class QRenderSettings;
  class QCamera;
}

namespace Qt3DExtras
{
  class QForwardRenderer;
}

class CameraController;
class Map3D;
class Terrain;

/**
 * Entity that encapsulates our 3D scene - contains all other entities (such as terrain) as children.
 */
class Scene : public Qt3DCore::QEntity
{
public:
  Scene(const Map3D& map, Qt3DExtras::QForwardRenderer *defaultFrameGraph, Qt3DRender::QRenderSettings *renderSettings, Qt3DRender::QCamera *camera, const QRect& viewportRect, Qt3DCore::QNode* parent = nullptr);

  CameraController* cameraController() { return mCameraController; }
  Terrain* terrain() { return mTerrain; }

private:
  CameraController* mCameraController;
  Terrain* mTerrain;
};

#endif // SCENE_H
