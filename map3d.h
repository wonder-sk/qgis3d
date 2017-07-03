#ifndef MAP3D_H
#define MAP3D_H

#include "tilingscheme.h"

class MapTextureGenerator;
class TerrainTextureGenerator;

class QgsMapLayer;
class QgsRasterLayer;

#include "qgscoordinatetransform.h"


//! Definition of the world
struct Map3D
{
  double originX, originY, originZ;   //!< coordinates in map CRS at which our 3D world has origin (0,0,0)
  QgsCoordinateReferenceSystem crs;   //!< destination coordinate system of the world  (TODO: not needed? can be
  double zExaggeration;

  QList<QgsMapLayer*> layers;   //!< layers to be rendered
  int tileTextureSize;   //!< size of map textures of tiles in pixels (width/height)
  MapTextureGenerator* mapGen;

  enum TerrainType { Flat, Dem, QuantizedMesh } terrainType;
  TilingScheme terrainTilingScheme;   //!< tiling scheme of the terrain

  QgsCoordinateTransform ctTerrainToMap;

  // TODO: terrain generator

  // TODO: this is used just for quantized mesh tiles
  int terrainBaseX, terrainBaseY, terrainBaseZ;   //!< coordinates of the base tile

  // TODO: this is used just for DEM terrain tiles
  TerrainTextureGenerator* tGen;
  int demTerrainSize;
  QgsRasterLayer* demLayer;
};


#endif // MAP3D_H
