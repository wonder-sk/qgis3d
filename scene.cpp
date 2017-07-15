#include "scene.h"

#include <Qt3DRender/QCamera>
#include <Qt3DRender/QPickingSettings>
#include <Qt3DRender/QRenderSettings>
#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DExtras/QSkyboxEntity>
#include <Qt3DLogic/QFrameAction>

#include "cameracontroller.h"
#include "lineentity.h"
#include "map3d.h"
#include "pointentity.h"
#include "polygonentity.h"
#include "terrain.h"


Scene::Scene(const Map3D& map, Qt3DExtras::QForwardRenderer *defaultFrameGraph, Qt3DRender::QRenderSettings *renderSettings, Qt3DRender::QCamera *camera, const QRect& viewportRect, Qt3DCore::QNode* parent)
  : Qt3DCore::QEntity(parent)
{
  defaultFrameGraph->setClearColor(map.backgroundColor);

#if QT_VERSION >= 0x050900
  // we want precise picking of terrain (also bounding volume picking does not seem to work - not sure why)
  renderSettings->pickingSettings()->setPickMethod(Qt3DRender::QPickingSettings::TrianglePicking);
#endif

  // Camera
  camera->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 10.f, 10000.0f);

  // Camera controlling
  mCameraController = new CameraController(this); // attaches to the scene
  mCameraController->setViewport(viewportRect);
  mCameraController->setCamera(camera);
  mCameraController->setCameraData(0, 0, 1000);

  // create terrain entity
  mTerrain = new Terrain(map);
  //t->setEnabled(false);
  mTerrain->setParent(this);
  mTerrain->setMaxLevel(3);
  mTerrain->setCamera(camera);
  connect(mCameraController, &CameraController::cameraChanged, mTerrain, &Terrain::cameraViewMatrixChanged);
  mTerrain->setViewport(viewportRect);
  // add camera control's terrain picker as a component to be able to capture height where mouse was
  // pressed in order to correcly pan camera when draggin mouse
  mTerrain->addComponent(mCameraController->terrainPicker());

  Q_FOREACH (const PolygonRenderer& pr, map.polygonRenderers)
  {
    PolygonEntity* p = new PolygonEntity(map, pr);
    p->setParent(this);
  }

  Qt3DCore::QEntity* lightEntity = new Qt3DCore::QEntity;
  Qt3DCore::QTransform* lightTransform = new Qt3DCore::QTransform;
  lightTransform->setTranslation(QVector3D(0, 1000, 0));
  Qt3DRender::QPointLight* light = new Qt3DRender::QPointLight;
  light->setColor(Qt::white);
  //light->setIntensity(0.5);
  lightEntity->addComponent(light);
  lightEntity->addComponent(lightTransform);
  lightEntity->setParent(this);

  Q_FOREACH (const PointRenderer& pr, map.pointRenderers)
  {
    PointEntity* pe = new PointEntity(map, pr);
    pe->setParent(this);
  }

  Q_FOREACH (const LineRenderer& lr, map.lineRenderers)
  {
    LineEntity* le = new LineEntity(map, lr);
    le->setParent(this);
  }

  if (map.skybox)
  {
    Qt3DExtras::QSkyboxEntity* skybox = new Qt3DExtras::QSkyboxEntity;
    skybox->setBaseName(map.skyboxFileBase);
    skybox->setExtension(map.skyboxFileExtension);
    skybox->setParent(this);

    // docs say frustum culling must be disabled for skybox.
    // it _somehow_ works even when frustum culling is enabled with some camera positions,
    // but then when zoomed in more it would disappear - so let's keep frustum culling disabled
    defaultFrameGraph->setFrustumCullingEnabled(false);
  }
}
