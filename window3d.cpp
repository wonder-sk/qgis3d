#include "window3d.h"

#include "qgsrectangle.h"

#include "flatterrain.h"
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

  Qt3DCore::QEntity *scene = createScene();

  // create terrain entity

  double tile0side = mapGen->getBaseTileSide();
  QgsRectangle extent(0, 0, tile0side, tile0side); // this is without map origin offset to avoid issues with x/y float precision

  FlatTerrain* t = new FlatTerrain(mapGen, extent);
  t->setParent( scene );
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



Qt3DCore::QEntity *Window3D::createScene()
{
  Qt3DCore::QEntity *rootEntity = new Qt3DCore::QEntity;

  //Qt3DExtras::QPlaneMesh* planeMesh = new Qt3DExtras::QPlaneMesh;
  MyMesh* planeMesh = new MyMesh;
  planeMesh->setWidth(50);
  planeMesh->setHeight(50);
  planeMesh->setMeshResolution(QSize(150,150));
  qDebug() << planeMesh->meshResolution();

  Qt3DCore::QTransform *planeTransform = new Qt3DCore::QTransform;
  planeTransform->setTranslation(QVector3D(0, -5, 0));

  Qt3DExtras::QDiffuseMapMaterial* planeMaterial = new Qt3DExtras::QDiffuseMapMaterial(rootEntity);
  Qt3DRender::QTexture2D* texture = new Qt3DRender::QTexture2D(planeMaterial);
  MapTextureImage* image = new MapTextureImage(mapGen, 0, 0, 0);
  texture->addTextureImage(image);
  texture->setMinificationFilter(Qt3DRender::QTexture2D::Linear);
  texture->setMagnificationFilter(Qt3DRender::QTexture2D::Linear);
  planeMaterial->setDiffuse(texture);
  planeMaterial->setAmbient(Qt::white);
  planeMaterial->setShininess(0);

  Qt3DCore::QEntity* planeEntity = new Qt3DCore::QEntity(rootEntity);
  planeEntity->addComponent(planeMesh);
  planeEntity->addComponent(planeMaterial);
  planeEntity->addComponent(planeTransform);

  planeEntity->setParent( (Qt3DCore::QNode*) nullptr );  // remove from the scene

  return rootEntity;
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
