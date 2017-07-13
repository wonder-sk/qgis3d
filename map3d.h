#ifndef MAP3D_H
#define MAP3D_H

#include <memory>
#include <QColor>

#include "qgscoordinatetransform.h"

class QgsMapLayer;
class QgsRasterLayer;
class QgsVectorLayer;

class MapTextureGenerator;
class TerrainGenerator;

struct PolygonRenderer
{
  QgsVectorLayer* layer;  //!< layer used to extract polygons from
  float height;           //!< base height of polygons
  float extrusionHeight;  //!< how much to extrude (0 means no walls)
  QColor ambientColor;
  QColor diffuseColor;
};

struct PointRenderer
{
  QgsVectorLayer* layer;  //!< layer used to extract points from
  float height;
  QColor diffuseColor;
};

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

  QList<PolygonRenderer> polygonRenderers;   //!< stuff to render as polygons
  QList<PointRenderer> pointRenderers;   //!< stuff to render as points

  bool skybox;  //!< whether to render skybox
  QString skyboxFileBase;
  QString skyboxFileExtension;
};


#endif // MAP3D_H
