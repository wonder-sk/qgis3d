#include "window3d.h"

#include "mymesh.h"
#include "cameracontroller.h"
#include "maptexturegenerator.h"
#include "sidepanel.h"


Window3D::Window3D(SidePanel* p, MapTextureGenerator* mapGen)
  : panel(p)
  , mapGen(mapGen)
{
  defaultFrameGraph()->setClearColor(QColor(Qt::black));

  Qt3DCore::QEntity *scene = createScene();

  mFrameAction = new Qt3DLogic::QFrameAction();
  connect(mFrameAction, &Qt3DLogic::QFrameAction::triggered,
          this, &Window3D::onFrameTriggered);
  scene->addComponent(mFrameAction);  // takes ownership

  // Camera
  camera()->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);

  // Camera controlling
  cc = new CameraController(scene); // attaches to the scene
  cc->setViewport(QRect(QPoint(0,0), size()));
  cc->setCamera(camera());

  connect(camera(), &Qt3DRender::QCamera::viewMatrixChanged, this, &Window3D::onCameraViewMatrixChanged);
  onCameraViewMatrixChanged();

  setRootEntity(scene);

  timer.start(1000);
  connect(&timer, &QTimer::timeout, this, &Window3D::onTimeout);
}


class MapTextureImageDataGenerator : public Qt3DRender::QTextureImageDataGenerator
{
public:
    int x, y, z;
    QImage img;

    static QImage placeholderImage()
    {
      // simple placeholder image
      QImage i(2, 2, QImage::Format_RGB32);
      i.setPixelColor(0, 0, Qt::darkGray);
      i.setPixelColor(1, 0, Qt::lightGray);
      i.setPixelColor(0, 1, Qt::lightGray);
      i.setPixelColor(1, 1, Qt::darkGray);
      return i;
    }

    MapTextureImageDataGenerator(int x, int y, int z, const QImage& img)
      : x(x), y(y), z(z), img(img) {}

    virtual Qt3DRender::QTextureImageDataPtr operator()() override
    {
      Qt3DRender::QTextureImageDataPtr dataPtr = Qt3DRender::QTextureImageDataPtr::create();
      dataPtr->setImage(img.isNull() ? placeholderImage() : img.mirrored());  // will copy image data to the internal byte array
      return dataPtr;
    }

    virtual bool operator ==(const QTextureImageDataGenerator &other) const override
    {
      const MapTextureImageDataGenerator *otherFunctor = functor_cast<MapTextureImageDataGenerator>(&other);
      return otherFunctor != nullptr && otherFunctor->img.isNull() == img.isNull() &&
          x == otherFunctor->x && y == otherFunctor->y && z == otherFunctor->z;
    }

    QT3D_FUNCTOR(MapTextureImageDataGenerator)
};


//! texture image with a rendered map
class MapTextureImage : public Qt3DRender::QAbstractTextureImage
{
public:
  MapTextureImage(MapTextureGenerator* mapGen, int x, int y, int z, Qt3DCore::QNode *parent = nullptr)
    : Qt3DRender::QAbstractTextureImage(parent)
    , mapGen(mapGen)
    , x(x), y(y), z(z)
  {
    connect(mapGen, &MapTextureGenerator::tileReady, [this, mapGen](int x, int y, int z, const QImage& img)
    {
      if (x == this->x && y == this->y && z == this->z)
      {
        this->img = img;
        this->notifyDataGeneratorChanged();
      }
    } );

    // request image
    mapGen->render(x, y, z);
  }

  virtual Qt3DRender::QTextureImageDataGeneratorPtr dataGenerator() const override
  {
    return Qt3DRender::QTextureImageDataGeneratorPtr(new MapTextureImageDataGenerator(x, y, z, img));
  }

  MapTextureGenerator* mapGen;
  int x, y, z;
  QImage img;
};

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
