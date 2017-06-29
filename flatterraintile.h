#ifndef FLATTERRAINTILE_H
#define FLATTERRAINTILE_H

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DExtras/QPlaneGeometry>
#include <Qt3DExtras/QTextureMaterial>

class MapTextureGenerator;
struct QuadTreeNode;
class FlatTerrainTileMesh;

//! base class for all kinds of terrain tiles
class TerrainTile : public Qt3DCore::QEntity
{
public:
  TerrainTile(QuadTreeNode* node, MapTextureGenerator* mapGen, Qt3DCore::QNode *parent = nullptr);

protected:
  Qt3DExtras::QTextureMaterial* material;
};

//! just a simple quad with a map texture
class FlatTerrainTile : public TerrainTile
{
public:
  FlatTerrainTile(QuadTreeNode* node, Qt3DExtras::QPlaneGeometry* tileGeometry, MapTextureGenerator* mapGen, Qt3DCore::QNode *parent = nullptr);

private:
  FlatTerrainTileMesh* mesh;
  Qt3DCore::QTransform* transform;
};


//! tile made from DEM
class DemTerrainTile : public TerrainTile
{
public:
  // TODO
};

#endif // FLATTERRAINTILE_H
