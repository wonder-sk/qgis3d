#ifndef MAPTEXTUREIMAGE_H
#define MAPTEXTUREIMAGE_H

#include <Qt3DRender/QAbstractTextureImage>

class MapTextureGenerator;

//! texture image with a rendered map
class MapTextureImage : public Qt3DRender::QAbstractTextureImage
{
public:
  MapTextureImage(MapTextureGenerator* mapGen, int x, int y, int z, Qt3DCore::QNode *parent = nullptr);

  virtual Qt3DRender::QTextureImageDataGeneratorPtr dataGenerator() const override;

  MapTextureGenerator* mapGen;
  int x, y, z;
  QImage img;
};

#endif // MAPTEXTUREIMAGE_H
