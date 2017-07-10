#ifndef DEMTERRAINGENERATOR_H
#define DEMTERRAINGENERATOR_H

#include "terraingenerator.h"

#include <memory>

class DemHeightMapGenerator;

class QgsRasterLayer;


/**
 * Implementation of terrain generator that uses a raster layer with DEM to build terrain.
 */
class DemTerrainGenerator : public TerrainGenerator
{
public:
  DemTerrainGenerator(QgsRasterLayer* dem, int terrainSize);

  Type type() const override;
  QgsRectangle extent() const override;
  virtual TerrainTileEntity* createTile(QuadTreeNode *n, const Map3D& map, Qt3DCore::QNode *parent) const override;

  std::unique_ptr<DemHeightMapGenerator> tGen;
  int demTerrainSize;
  QgsRasterLayer* demLayer;
};


class DemTerrainTileGeometry;

//! tile made from DEM
class DemTerrainTile : public TerrainTileEntity
{
  Q_OBJECT
public:
  DemTerrainTile(QuadTreeNode* node, const Map3D& map, Qt3DCore::QNode *parent = nullptr);

private slots:
  void onHeightMapReady(int jobId, const QByteArray& heightMap);

private:
  int jobId;
  DemTerrainTileGeometry* geometry;
};


#include <QtConcurrent/QtConcurrentRun>
#include <QFutureWatcher>

#include "qgsrectangle.h"

/**
 * Utility class to asynchronously create heightmaps from DEM raster for given tiles of terrain.
 */
class DemHeightMapGenerator : public QObject
{
  Q_OBJECT
public:
  DemHeightMapGenerator(QgsRasterLayer* dtm, const TilingScheme& tilingScheme, int resolution);
  ~DemHeightMapGenerator();

  //! asynchronous terrain read for a tile (array of floats)
  int render(int x, int y, int z);

  int resolution() const { return res; }

signals:
  //! emitted when a previously requested heightmap is ready
  void heightMapReady(int jobId, const QByteArray& heightMap);

private slots:
  void onFutureFinished();

private:
  //! raster used to build terrain
  QgsRasterLayer* dtm;

  TilingScheme tilingScheme;

  int res;

  int lastJobId;

  struct JobData
  {
    int jobId;
    QgsRectangle extent;
    QFuture<QByteArray> future;
    QFutureWatcher<QByteArray>* fw;
  };

  QHash<QFutureWatcher<QByteArray>*, JobData> jobs;
};

#endif // DEMTERRAINGENERATOR_H
