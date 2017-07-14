#include "map3d.h"

#include "flatterraingenerator.h"

Map3D::Map3D()
  : originX(0)
  , originY(0)
  , originZ(0)
  , zExaggeration(1)
  , backgroundColor(Qt::black)
  , tileTextureSize(512)
  , skybox(false)
{
}
