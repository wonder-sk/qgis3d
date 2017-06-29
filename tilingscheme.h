#ifndef TILINGSCHEME_H
#define TILINGSCHEME_H

#include <qgspointxy.h>

class QgsRectangle;

//! The class encapsulates tiling scheme (just like with WMTS / TMS / XYZ layers).
//! The origin (tile [0,0]) is in bottom-left corner.
class TilingScheme
{
public:
  //! Creates invalid tiling scheme
  TilingScheme();

  //! Creates tiling scheme where level 0 tile is centered at the full extent and the full extent completely fits into the level 0 tile
  TilingScheme(const QgsRectangle& fullExtent);

  //! Returns map coordinates at tile coordinates (for lower-left corner of the tile)
  QgsPointXY tileToMap(int x, int y, int z);
  //! Returns tile coordinates for given map coordinates and Z level
  void mapToTile(const QgsPointXY& pt, int z, float& x, float& y);

  //! Returns map coordinates of the extent of a tile
  QgsRectangle tileToExtent(int x, int y, int z);

  QgsPointXY mapOrigin; //!< origin point in map coordinates: (0,0) in the tiling scheme
  double baseTileSide;  //!< length of tile side at zoom level 0 in map coordinates

};

#endif // TILINGSCHEME_H
