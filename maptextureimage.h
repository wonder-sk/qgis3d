#ifndef MAPTEXTUREIMAGE_H
#define MAPTEXTUREIMAGE_H

#include <Qt3DRender/QAbstractTextureImage>

#include "qgsrectangle.h"

class MapTextureGenerator;

//! texture image with a rendered map
class MapTextureImage : public Qt3DRender::QAbstractTextureImage
{
public:
  MapTextureImage(MapTextureGenerator* mapGen, const QgsRectangle& extent, const QString& debugText = QString(), Qt3DCore::QNode *parent = nullptr);

  virtual Qt3DRender::QTextureImageDataGeneratorPtr dataGenerator() const override;

  MapTextureGenerator* mapGen;
  QgsRectangle extent;
  QString debugText;
  QImage img;
};

#endif // MAPTEXTUREIMAGE_H
