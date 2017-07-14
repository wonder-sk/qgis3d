#ifndef MAP3D_H
#define MAP3D_H

#include <memory>
#include <QColor>
#include <QMatrix4x4>

#include "qgscoordinatereferencesystem.h"
#include "qgsmaplayerref.h"

class QgsMapLayer;
class QgsRasterLayer;
class QgsVectorLayer;

class MapTextureGenerator;
class TerrainGenerator;

class PolygonRenderer
{
public:
  PolygonRenderer();

  void setLayer(QgsVectorLayer* layer);
  QgsVectorLayer *layer() const;

  void writeXml(QDomElement& elem) const;
  void readXml(const QDomElement& elem);
  void resolveReferences(const QgsProject& project);

  float height;           //!< base height of polygons
  float extrusionHeight;  //!< how much to extrude (0 means no walls)
  QColor ambientColor;
  QColor diffuseColor;

private:
  QgsMapLayerRef layerRef; //!< layer used to extract polygons from
};

class PointRenderer
{
public:
  PointRenderer();

  void setLayer(QgsVectorLayer* layer);
  QgsVectorLayer *layer() const;

  void writeXml(QDomElement& elem) const;
  void readXml(const QDomElement& elem);
  void resolveReferences(const QgsProject& project);

  float height;
  QColor diffuseColor;
  QVariantMap shapeProperties;  //!< what kind of shape to use and what
  QMatrix4x4 transform;  //!< transform of individual instanced models

private:
  QgsMapLayerRef layerRef; //!< layer used to extract points from
};

class QgsReadWriteContext;
class QgsProject;

class QDomElement;

//! Definition of the world
class Map3D
{
public:
  Map3D();

  void readXml( const QDomElement& elem, const QgsReadWriteContext& context );

  QDomElement writeXml( QDomDocument& doc, const QgsReadWriteContext& context ) const;

  void resolveReferences(const QgsProject& project);

  double originX, originY, originZ;   //!< coordinates in map CRS at which our 3D world has origin (0,0,0)
  QgsCoordinateReferenceSystem crs;   //!< destination coordinate system of the world  (TODO: not needed? can be
  QColor backgroundColor;   //!< background color of the scene

  //
  // terrain related config
  //

  double zExaggeration;   //!< multiplier of terrain heights to make the terrain shape more pronounced

  void setLayers(const QList<QgsMapLayer*>& layers);
  QList<QgsMapLayer*> layers() const;

  int tileTextureSize;   //!< size of map textures of tiles in pixels (width/height)
  std::unique_ptr<TerrainGenerator> terrainGenerator;  //!< implementation of the terrain generation

  //
  // 3D renderers
  //

  QList<PolygonRenderer> polygonRenderers;   //!< stuff to render as polygons
  QList<PointRenderer> pointRenderers;   //!< stuff to render as points

  bool skybox;  //!< whether to render skybox
  QString skyboxFileBase;
  QString skyboxFileExtension;

  bool showBoundingBoxes;  //!< whether to show bounding boxes of entities - useful for debugging
  bool drawTerrainTileInfo;  //!< whether to draw extra information about terrain tiles to the textures - useful for debugging

private:
  QList<QgsMapLayerRef> mLayers;   //!< layers to be rendered
};


#endif // MAP3D_H
