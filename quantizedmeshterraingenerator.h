#ifndef QUANTIZEDMESHTERRAINGENERATOR_H
#define QUANTIZEDMESHTERRAINGENERATOR_H

#include "terraingenerator.h"
#include "terrainchunkloader.h"


class QuantizedMeshTerrainGenerator : public TerrainGenerator, public ChunkLoaderFactory
{
public:
  QuantizedMeshTerrainGenerator();

  void setBaseTileFromExtent(const QgsRectangle& extentInTerrainCrs);

  void quadTreeTileToBaseTile(int x, int y, int z, int &tx, int &ty, int &tz) const;

  TerrainGenerator::Type type() const override;
  QgsRectangle extent() const override;
  TerrainTileEntity* createTile(Terrain* terrain, QuadTreeNode *n, Qt3DCore::QNode *parent) const override;
  virtual void writeXml(QDomElement& elem) const override;
  virtual void readXml(const QDomElement& elem) override;

  virtual ChunkLoader* createChunkLoader(ChunkNode* node) const override;

  int terrainBaseX, terrainBaseY, terrainBaseZ;   //!< coordinates of the base tile
};


#endif // QUANTIZEDMESHTERRAINGENERATOR_H
