#include "maptexturegenerator.h"

#include <qgsmaprenderersequentialjob.h>
#include <qgsmapsettings.h>
#include <qgsproject.h>

#include "map3d.h"

MapTextureGenerator::MapTextureGenerator(const Map3D& map)
  : map(map)
{
}

void MapTextureGenerator::render(const QgsRectangle &extent, const QString &debugText)
{
  QgsMapSettings mapSettings(baseMapSettings());
  mapSettings.setExtent(extent);

  QgsMapRendererSequentialJob* job = new QgsMapRendererSequentialJob(mapSettings);
  connect(job, &QgsMapRendererJob::finished, this, &MapTextureGenerator::onRenderingFinished);
  job->start();

  JobData jobData;
  jobData.job = job;
  jobData.extent = extent;
  jobData.debugText = debugText;

  jobs.insert(job, jobData);
}


void MapTextureGenerator::onRenderingFinished()
{
  QgsMapRendererSequentialJob* mapJob = static_cast<QgsMapRendererSequentialJob*>(sender());

  Q_ASSERT(jobs.contains(mapJob));
  JobData jobData = jobs.value(mapJob);

  QImage img = mapJob->renderedImage();

  // extra tile information for debugging
  QPainter p(&img);
  p.setPen(Qt::white);
  p.drawRect(0,0,img.width()-1, img.height()-1);
  p.drawText(img.rect(), jobData.debugText, QTextOption(Qt::AlignCenter));
  p.end();

  mapJob->deleteLater();
  jobs.remove(mapJob);

  // pass QImage further
  emit tileReady(jobData.extent, img);
}

QgsMapSettings MapTextureGenerator::baseMapSettings()
{
  QgsMapSettings mapSettings;
  mapSettings.setLayers(map.layers);
  mapSettings.setOutputSize(QSize(map.tileTextureSize,map.tileTextureSize));
  mapSettings.setDestinationCrs(map.crs);
  mapSettings.setBackgroundColor(Qt::gray);
  return mapSettings;
}


// ---------------------

#include <qgsrasterlayer.h>

TerrainTextureGenerator::TerrainTextureGenerator(QgsRasterLayer *dtm, const TilingScheme &tilingScheme, int resolution)
  : dtm(dtm)
  , tilingScheme(tilingScheme)
  , res(resolution)
{
}

QByteArray TerrainTextureGenerator::render(int x, int y, int z)
{
  // extend the rect by half-pixel on each side? to get the values in "corners"
  QgsRectangle extent = tilingScheme.tileToExtent(x, y, z);
  float mapUnitsPerPixel = extent.width() / res;
  extent.grow( mapUnitsPerPixel / 2);
  // but make sure not to go beyond the full extent (returns invalid values)
  QgsRectangle fullExtent = tilingScheme.tileToExtent(0, 0, 0);
  extent = extent.intersect(&fullExtent);

  std::unique_ptr<QgsRasterBlock> block( dtm->dataProvider()->block(1, extent, res, res) );
  if (!block)
    return QByteArray();
  block->convert(Qgis::Float32);   // currently we expect just floats
  QByteArray data = block->data();
  data.data();  // this should make a deep copy
  return data;
}
