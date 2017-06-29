#ifndef FLATTERRAINTILE_H
#define FLATTERRAINTILE_H

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DExtras/QPlaneGeometry>
#include <Qt3DExtras/QTextureMaterial>

class MapTextureGenerator;
class TerrainTextureGenerator;
struct QuadTreeNode;
class FlatTerrainTileMesh;

//! base class for all kinds of terrain tiles
class TerrainTile : public Qt3DCore::QEntity
{
public:
  TerrainTile(QuadTreeNode* node, MapTextureGenerator* mapGen, Qt3DCore::QNode *parent = nullptr);

protected:
  Qt3DExtras::QTextureMaterial* material;
  Qt3DCore::QTransform* transform;
};

//! just a simple quad with a map texture
class FlatTerrainTile : public TerrainTile
{
public:
  FlatTerrainTile(Qt3DExtras::QPlaneGeometry* tileGeometry, QuadTreeNode* node, MapTextureGenerator* mapGen, Qt3DCore::QNode *parent = nullptr);

private:
  FlatTerrainTileMesh* mesh;
};



//! tile made from DEM
class DemTerrainTile : public TerrainTile
{
public:
  DemTerrainTile(TerrainTextureGenerator* tGen, QuadTreeNode* node, MapTextureGenerator* mapGen, Qt3DCore::QNode *parent = nullptr);

private:
  //Qt3DRender::QGeometryRenderer* mesh;
};

#endif // FLATTERRAINTILE_H
