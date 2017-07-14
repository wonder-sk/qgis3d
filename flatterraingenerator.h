#ifndef FLATTERRAINGENERATOR_H
#define FLATTERRAINGENERATOR_H

#include "terraingenerator.h"

namespace Qt3DExtras
{
  class QPlaneGeometry;
}

class FlatTerrainGenerator : public TerrainGenerator
{
public:
  FlatTerrainGenerator();

  Type type() const override;
  QgsRectangle extent() const override;
  TerrainTileEntity* createTile(Terrain* terrain, QuadTreeNode *n, Qt3DCore::QNode *parent) const override;

  void setExtent(const QgsRectangle& extent, const QgsCoordinateReferenceSystem& crs);

private:
  Qt3DExtras::QPlaneGeometry* tileGeometry;
};




#endif // FLATTERRAINGENERATOR_H
