#include "maptextureimage.h"

#include <Qt3DRender/QTextureImageDataGenerator>

#include "maptexturegenerator.h"

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
      dataPtr->setImage(img.isNull() ? placeholderImage() : img); // will copy image data to the internal byte array
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


MapTextureImage::MapTextureImage(MapTextureGenerator *mapGen, int x, int y, int z, Qt3DCore::QNode *parent)
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

Qt3DRender::QTextureImageDataGeneratorPtr MapTextureImage::dataGenerator() const
{
  return Qt3DRender::QTextureImageDataGeneratorPtr(new MapTextureImageDataGenerator(x, y, z, img));
}
