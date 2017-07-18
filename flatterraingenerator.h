#ifndef FLATTERRAINGENERATOR_H
#define FLATTERRAINGENERATOR_H

#include "terraingenerator.h"

#include "qgsrectangle.h"

#include "chunkloader.h"

namespace Qt3DExtras
{
  class QPlaneGeometry;
}

class FlatTerrainGenerator : public TerrainGenerator, public ChunkLoaderFactory
{
public:
  FlatTerrainGenerator();

  virtual ChunkLoader *createChunkLoader(ChunkNode* node) const override;

  Type type() const override;
  QgsRectangle extent() const override;
  TerrainTileEntity* createTile(Terrain* terrain, QuadTreeNode *n, Qt3DCore::QNode *parent) const override;
  virtual void writeXml(QDomElement& elem) const override;
  virtual void readXml(const QDomElement& elem) override;

  void setExtent(const QgsRectangle& extent);

  void setCrs(const QgsCoordinateReferenceSystem& crs);
  QgsCoordinateReferenceSystem crs() const { return mCrs; }

private:
  Qt3DExtras::QPlaneGeometry* tileGeometry;

  void updateTilingScheme();

  QgsRectangle mExtent;
  QgsCoordinateReferenceSystem mCrs;
};




#endif // FLATTERRAINGENERATOR_H
