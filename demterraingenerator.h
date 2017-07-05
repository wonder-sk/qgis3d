#ifndef DEMTERRAINGENERATOR_H
#define DEMTERRAINGENERATOR_H

#include "terraingenerator.h"

#include <memory>

class TerrainTextureGenerator;

class QgsRasterLayer;


class DemTerrainGenerator : public TerrainGenerator
{
public:
  DemTerrainGenerator(QgsRasterLayer* dem, int terrainSize);

  Type type() const override;
  QgsRectangle extent() const override;
  virtual TerrainTileEntity* createTile(QuadTreeNode *n, const Map3D& map, Qt3DCore::QNode *parent) const override;

  std::unique_ptr<TerrainTextureGenerator> tGen;
  int demTerrainSize;
  QgsRasterLayer* demLayer;
};

#endif // DEMTERRAINGENERATOR_H
