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

//! Basic shading material used for rendering
class PhongMaterialSettings
{
public:
  PhongMaterialSettings()
    : mAmbient(QColor::fromRgbF(0.05f, 0.05f, 0.05f, 1.0f))
    , mDiffuse(QColor::fromRgbF(0.7f, 0.7f, 0.7f, 1.0f))
    , mSpecular(QColor::fromRgbF(0.01f, 0.01f, 0.01f, 1.0f))
    , mShininess(150.0f)
  {
  }

  QColor ambient() const { return mAmbient; }
  QColor diffuse() const { return mDiffuse; }
  QColor specular() const { return mSpecular; }
  float shininess() const { return mShininess; }

  void setAmbient(const QColor &ambient) { mAmbient = ambient; }
  void setDiffuse(const QColor &diffuse) { mDiffuse = diffuse; }
  void setSpecular(const QColor &specular) { mSpecular = specular; }
  void setShininess(float shininess) { mShininess = shininess; }

  void readXml( const QDomElement& elem );
  void writeXml( QDomElement& elem ) const;

private:
  QColor mAmbient;
  QColor mDiffuse;
  QColor mSpecular;
  float mShininess;
};

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
  PhongMaterialSettings material;  //!< defines appearance of objects

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
  PhongMaterialSettings material;  //!< defines appearance of objects
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
