#ifndef MAPTEXTUREGENERATOR_H
#define MAPTEXTUREGENERATOR_H

class QgsMapRendererSequentialJob;
class QgsMapSettings;
class QgsProject;
class QgsRasterLayer;

#include <QObject>

#include <qgspointxy.h>

/**
 * Responsible for:
 * - rendering map tiles in background
 * - caching tiles on disk (?)
 */
class MapTextureGenerator : public QObject
{
  Q_OBJECT
public:
  MapTextureGenerator(QgsProject* project);

  //! start async rendering of a tile
  void render(int x, int y, int z);

  double getBaseTileSide() const { return baseTileSide; }

signals:
  void tileReady(int x, int y, int z, const QImage& image);

private slots:
  void onRenderingFinished();

private:
  QgsMapSettings baseMapSettings();
  QgsMapSettings mapSettingsForTile(int x, int y, int z);

  QgsPointXY tileToMap(int x, int y, int z);
  void mapToTile(const QgsPointXY& pt, int z, float& x, float& y);

  //! project to be rendered into map
  QgsProject* project;

  QgsPointXY mapOrigin; //!< origin point in map coordinates: (0,0) in the tiling scheme
  double baseTileSide;  //!< length of tile side at zoom level 0 in map coordinates

  struct JobData
  {
    QgsMapRendererSequentialJob* job;
    int x, y, z;
  };

  QHash<QgsMapRendererSequentialJob*, JobData> jobs;
};


/**
 * Responsible for creating mesh geometry from raster DTM
 */
class TerrainGenerator
{
public:
  TerrainGenerator(QgsRasterLayer* dtm);

  //! raster used to build terrain
  QgsRasterLayer* dtm;
};


#endif // MAPTEXTUREGENERATOR_H
