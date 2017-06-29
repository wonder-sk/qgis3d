#ifndef MAPTEXTUREGENERATOR_H
#define MAPTEXTUREGENERATOR_H

class QgsMapRendererSequentialJob;
class QgsMapSettings;
class QgsProject;
class QgsRasterLayer;

#include <QObject>

#include "tilingscheme.h"


/**
 * Responsible for:
 * - rendering map tiles in background
 * - caching tiles on disk (?)
 */
class MapTextureGenerator : public QObject
{
  Q_OBJECT
public:
  MapTextureGenerator(QgsProject* project, const TilingScheme& tilingScheme, int resolution);

  //! start async rendering of a tile
  void render(int x, int y, int z);

  const TilingScheme& getTilingScheme() const { return tilingScheme; }

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

  TilingScheme tilingScheme;

  int res;

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
class TerrainTextureGenerator
{
public:
  TerrainTextureGenerator(QgsRasterLayer* dtm, const TilingScheme& tilingScheme, int resolution);

  //! synchronous terrain read for a tile (array of floats)
  QByteArray render(int x, int y, int z);

  int resolution() const { return res; }

private:
  //! raster used to build terrain
  QgsRasterLayer* dtm;

  TilingScheme tilingScheme;

  int res;
};


#endif // MAPTEXTUREGENERATOR_H
