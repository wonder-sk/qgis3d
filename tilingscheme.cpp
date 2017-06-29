#include "tilingscheme.h"

#include "qgsrectangle.h"


TilingScheme::TilingScheme()
  : mapOrigin()
  , baseTileSide(0)
{
}

TilingScheme::TilingScheme(const QgsRectangle &fullExtent)
{
  mapOrigin = QgsPointXY(fullExtent.xMinimum(), fullExtent.yMinimum());
  baseTileSide = qMax(fullExtent.width(), fullExtent.height());
}

QgsPointXY TilingScheme::tileToMap(int x, int y, int z)
{
  double tileSide = baseTileSide / pow(2, z);
  double mx = mapOrigin.x() + x * tileSide;
  double my = mapOrigin.y() + y * tileSide;
  return QgsPointXY(mx, my);
}

void TilingScheme::mapToTile(const QgsPointXY &pt, int z, float &x, float &y)
{
  double tileSide = baseTileSide / pow(2, z);
  x = (pt.x() - mapOrigin.x()) / tileSide;
  y = (pt.y() - mapOrigin.y()) / tileSide;
}

QgsRectangle TilingScheme::tileToExtent(int x, int y, int z)
{
  QgsPointXY pt0 = tileToMap(x, y, z);
  QgsPointXY pt1 = tileToMap(x+1, y+1, z);
  return QgsRectangle(pt0, pt1);
}
