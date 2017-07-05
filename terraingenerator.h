#ifndef FLATTERRAINTILE_H
#define FLATTERRAINTILE_H

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DExtras/QTextureMaterial>

#include "aabb.h"
#include "tilingscheme.h"

struct QuadTreeNode;
class FlatTerrainTileMesh;
class Map3D;
class QgsRectangle;


/**
 * Base class for all kinds of terrain tiles.
 * This is an Qt 3D entity meant for inclusion in the 3D scene.
 *
 * It automatically creates transform component and texture material
 * with texture rendered from the map's layers.
 */
class TerrainTileEntity : public Qt3DCore::QEntity
{
public:
  TerrainTileEntity(QuadTreeNode* node, const Map3D& map, Qt3DCore::QNode *parent = nullptr);

  AABB bbox;  //!< bounding box
  float epsilon;  //!< (geometric) error of this tile (in world coordinates)

protected:
  Qt3DExtras::QTextureMaterial* material;
  Qt3DCore::QTransform* transform;
};


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

  //! What texture generator implementation is this
  virtual Type type() const = 0;

  //! extent of the terrain in terrain's CRS
  virtual QgsRectangle extent() const = 0;

  //! Factory method to create tile entity for given [x,y,z] tile coordinates
  virtual TerrainTileEntity* createTile(QuadTreeNode *n, const Map3D& map, Qt3DCore::QNode *parent) const = 0;

  QgsCoordinateReferenceSystem crs() const { return terrainTilingScheme.crs; }

  TilingScheme terrainTilingScheme;   //!< tiling scheme of the terrain
};


#endif // FLATTERRAINTILE_H
