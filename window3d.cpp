#include "window3d.h"

#include "qgsrectangle.h"

#include "terrain.h"
#include "mymesh.h"
#include "cameracontroller.h"
#include "maptexturegenerator.h"
#include "maptextureimage.h"
#include "sidepanel.h"


Window3D::Window3D(SidePanel* p, MapTextureGenerator* mapGen)
  : panel(p)
  , mapGen(mapGen)
{
  defaultFrameGraph()->setClearColor(QColor(Qt::black));

  Qt3DCore::QEntity *scene = new Qt3DCore::QEntity;

  // create terrain entity

  double tile0side = mapGen->getBaseTileSide();
  QgsRectangle extent(0, 0, tile0side, tile0side); // this is without map origin offset to avoid issues with x/y float precision

  Terrain* t = new Terrain(mapGen, extent);
  t->setParent( scene );
  t->setFlat(true);
  t->setMaxLevel(4);
  t->setCamera( camera() );

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
  cc->setCameraData(extent.center().x(), -extent.center().y(), tile0side);

  connect(camera(), &Qt3DRender::QCamera::viewMatrixChanged, this, &Window3D::onCameraViewMatrixChanged);
  onCameraViewMatrixChanged();

  setRootEntity(scene);

  timer.start(1000);
  connect(&timer, &QTimer::timeout, this, &Window3D::onTimeout);
}


void Window3D::resizeEvent(QResizeEvent *ev)
{
  Qt3DExtras::Qt3DWindow::resizeEvent(ev);
  cc->setViewport(QRect(QPoint(0,0), size()));
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
