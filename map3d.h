#ifndef MAP3D_H
#define MAP3D_H

#include <memory>

#include "qgscoordinatetransform.h"

class QgsMapLayer;
class QgsRasterLayer;

class MapTextureGenerator;
class TerrainGenerator;


//! Definition of the world
struct Map3D
{
  double originX, originY, originZ;   //!< coordinates in map CRS at which our 3D world has origin (0,0,0)
  QgsCoordinateReferenceSystem crs;   //!< destination coordinate system of the world  (TODO: not needed? can be
  double zExaggeration;

  QList<QgsMapLayer*> layers;   //!< layers to be rendered
  int tileTextureSize;   //!< size of map textures of tiles in pixels (width/height)
  MapTextureGenerator* mapGen;

  QgsCoordinateTransform ctTerrainToMap;

  std::unique_ptr<TerrainGenerator> terrainGenerator;
};


#endif // MAP3D_H
