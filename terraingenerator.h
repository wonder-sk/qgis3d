#ifndef FLATTERRAINTILE_H
#define FLATTERRAINTILE_H

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>

#if QT_VERSION >= 0x050900
#include <Qt3DExtras/QTextureMaterial>
#else
#include <Qt3DExtras/QDiffuseMapMaterial>
#endif

#include "aabb.h"
#include "tilingscheme.h"

class QuadTreeNode;
class FlatTerrainTileMesh;
class Map3D;
class QgsRectangle;
class Terrain;

/**
 * Base class for all kinds of terrain tiles.
 * This is an Qt 3D entity meant for inclusion in the 3D scene.
 *
 * It automatically creates transform component and texture material
 * with texture rendered from the map's layers.
 */
class TerrainTileEntity : public Qt3DCore::QEntity
{
  Q_OBJECT
public:
  TerrainTileEntity(Terrain* terrain, QuadTreeNode* node, Qt3DCore::QNode *parent = nullptr);

  Terrain* mTerrain;
  AABB bbox;  //!< bounding box
  float epsilon;  //!< (geometric) error of this tile (in world coordinates)
  bool m_textureReady;  //!< whether tile's texture is available -> ready to be displayed

private slots:
  void onTextureReady();

signals:
  void textureReady();

protected:
#if QT_VERSION >= 0x050900
  Qt3DExtras::QTextureMaterial* material;
#else
  Qt3DExtras::QDiffuseMapMaterial* material;
#endif
  Qt3DCore::QTransform* transform;
};

class QDomElement;
class QDomDocument;
class QgsProject;

/**
 * Base class for generators of terrain. All terrain generators are tile based
 * to support hierarchical level of detail. Tiling scheme of a generator is defined
 * by the generator itself. Terrain generators are asked to produce new terrain tiles
 * whenever that is deemed necessary by the terrain controller (that caches generated tiles).
 */
class TerrainGenerator
{
public:

  enum Type
  {
    Flat,
    Dem,
    QuantizedMesh
  };

  virtual ~TerrainGenerator() {}

  void setTerrain(Terrain* t) { mTerrain = t; }

  //! What texture generator implementation is this
  virtual Type type() const = 0;

  //! extent of the terrain in terrain's CRS
  virtual QgsRectangle extent() const = 0;

  //! Factory method to create tile entity for given [x,y,z] tile coordinates
  virtual TerrainTileEntity* createTile(Terrain* terrain, QuadTreeNode *n, Qt3DCore::QNode *parent) const = 0;

  //! Write terrain generator's configuration to XML
  virtual void writeXml(QDomElement& elem) const = 0;

  //! Read terrain generator's configuration from XML
  virtual void readXml(const QDomElement& elem) = 0;

  //! After read of XML, resolve references to any layers that have been read as layer IDs
  virtual void resolveReferences(const QgsProject& project) { Q_UNUSED(project); }

  static QString typeToString(Type type);

  QgsCoordinateReferenceSystem crs() const { return terrainTilingScheme.crs; }

  TilingScheme terrainTilingScheme;   //!< tiling scheme of the terrain

  Terrain* mTerrain = nullptr;
};


#endif // FLATTERRAINTILE_H
