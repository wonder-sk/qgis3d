#include "window3d.h"

#include "qgsrectangle.h"

#include "map3d.h"
#include "terrain.h"
#include "cameracontroller.h"
#include "maptexturegenerator.h"
#include "maptextureimage.h"
#include "terraingenerator.h"
#include "sidepanel.h"

#include "polygonentity.h"

Window3D::Window3D(SidePanel* p, Map3D& map)
  : panel(p)
  , map(map)
{
  defaultFrameGraph()->setClearColor(QColor(Qt::black));

  Qt3DCore::QEntity *scene = new Qt3DCore::QEntity;

#if QT_VERSION >= 0x050900
  // we want precise picking of terrain (also bounding volume picking does not seem to work - not sure why)
  renderSettings()->pickingSettings()->setPickMethod(Qt3DRender::QPickingSettings::TrianglePicking);
#endif

  mFrameAction = new Qt3DLogic::QFrameAction();
  connect(mFrameAction, &Qt3DLogic::QFrameAction::triggered,
          this, &Window3D::onFrameTriggered);
  scene->addComponent(mFrameAction);  // takes ownership

  // Camera
  camera()->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 10.f, 10000.0f);

  // Camera controlling
  cc = new CameraController(scene); // attaches to the scene
  cc->setViewport(QRect(QPoint(0,0), size()));
  cc->setCamera(camera());
  cc->setCameraData(0, 0, 1000);

  connect(camera(), &Qt3DRender::QCamera::viewMatrixChanged, this, &Window3D::onCameraViewMatrixChanged);
  onCameraViewMatrixChanged();

  // create terrain entity
  terrain = new Terrain(map, map.terrainGenerator->extent());
  //t->setEnabled(false);
  terrain->setParent( scene );
  terrain->setMaxLevel(3);
  terrain->setCamera( camera() );
  connect(cc, &CameraController::cameraChanged, terrain, &Terrain::cameraViewMatrixChanged);
  terrain->setViewport(QRect(QPoint(0,0), size()));
  // add camera control's terrain picker as a component to be able to capture height where mouse was
  // pressed in order to correcly pan camera when draggin mouse
  terrain->addComponent(cc->terrainPicker());

  Q_FOREACH (const PolygonRenderer& pr, map.polygonRenderers)
  {
    PolygonEntity* p = new PolygonEntity(map, pr);
    p->setParent(scene);
  }

  Qt3DCore::QEntity* lightEntity = new Qt3DCore::QEntity;
  Qt3DCore::QTransform* lightTransform = new Qt3DCore::QTransform;
  lightTransform->setTranslation(QVector3D(0, 1000, 0));
  Qt3DRender::QPointLight* light = new Qt3DRender::QPointLight;
  light->setColor(Qt::white);
  //light->setIntensity(0.5);
  lightEntity->addComponent(light);
  lightEntity->addComponent(lightTransform);
  lightEntity->setParent(scene);

  setRootEntity(scene);

  timer.start(1000);
  connect(&timer, &QTimer::timeout, this, &Window3D::onTimeout);
}


void Window3D::resizeEvent(QResizeEvent *ev)
{
  Qt3DExtras::Qt3DWindow::resizeEvent(ev);
  cc->setViewport(QRect(QPoint(0,0), size()));
  terrain->setViewport(QRect(QPoint(0,0), size()));
}

void Window3D::onTimeout()
{
  panel->setFps(frames);
  frames = 0;
}

void Window3D::onFrameTriggered(float dt)
{
  Q_UNUSED(dt);
  //qDebug() << dt*1000;
  frames++;
}

void Window3D::onCameraViewMatrixChanged()
{
  //QMatrix4x4 v = camera()->viewMatrix();
  //v.scale(QVector3D(1,3,1));
  //QMatrix4x4 m = v.inverted();
  //txtTransform->setMatrix(m);
}
