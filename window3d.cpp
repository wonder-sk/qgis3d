#include "window3d.h"

#include "mymesh.h"
#include "cameracontroller.h"

Window3D::Window3D()
{
  Qt3DCore::QEntity *scene = createScene();

  // Camera
  camera()->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);

  // Camera controlling
  cc = new CameraController(scene); // attaches to the scene
  cc->setViewport(QRect(QPoint(0,0), size()));
  cc->setCamera(camera());

  setRootEntity(scene);
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
  Qt3DRender::QTextureImage* image = new Qt3DRender::QTextureImage();
  image->setSource(QUrl("qrc:/ap.tif"));
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

  return rootEntity;
}

void Window3D::resizeEvent(QResizeEvent *ev)
{
  Qt3DExtras::Qt3DWindow::resizeEvent(ev);
  cc->setViewport(QRect(QPoint(0,0), size()));
}
