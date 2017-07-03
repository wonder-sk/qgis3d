#ifndef MAPTEXTUREGENERATOR_H
#define MAPTEXTUREGENERATOR_H

class QgsMapRendererSequentialJob;
class QgsMapSettings;
class QgsProject;
class QgsRasterLayer;

#include <QObject>

#include "tilingscheme.h"

#include "qgsrectangle.h"

struct Map3D;

/**
 * Responsible for:
 * - rendering map tiles in background
 * - caching tiles on disk (?)
 */
class MapTextureGenerator : public QObject
{
  Q_OBJECT
public:
  MapTextureGenerator(const Map3D& map);

  //! Start async rendering of a map for the given extent (must be a square!)
  void render(const QgsRectangle& extent, const QString& debugText = QString());

signals:
  void tileReady(const QgsRectangle& extent, const QImage& image);

private slots:
  void onRenderingFinished();

private:
  QgsMapSettings baseMapSettings();

  const Map3D& map;

  struct JobData
  {
    QgsMapRendererSequentialJob* job;
    QgsRectangle extent;
    QString debugText;
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
