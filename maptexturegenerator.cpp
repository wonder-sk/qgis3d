#include "maptexturegenerator.h"

#include <qgsmaprenderersequentialjob.h>
#include <qgsmapsettings.h>
#include <qgsproject.h>

MapTextureGenerator::MapTextureGenerator(QgsProject* project, const TilingScheme& tilingScheme, int resolution)
  : project(project)
  , tilingScheme(tilingScheme)
  , res(resolution)
{
}

void MapTextureGenerator::render(int x, int y, int z)
{
  QgsMapSettings mapSettings = mapSettingsForTile(x, y, z);

  QgsMapRendererSequentialJob* job = new QgsMapRendererSequentialJob(mapSettings);
  connect(job, &QgsMapRendererJob::finished, this, &MapTextureGenerator::onRenderingFinished);
  job->start();

  JobData jobData;
  jobData.job = job;
  jobData.x = x;
  jobData.y = y;
  jobData.z = z;

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
  QString tileText = QString("%1 | %2 | %3").arg(jobData.x).arg(jobData.y).arg(jobData.z);
  p.drawRect(0,0,img.width()-1, img.height()-1);
  p.drawText(img.rect(), tileText, QTextOption(Qt::AlignCenter));
  p.end();

  mapJob->deleteLater();
  jobs.remove(mapJob);

  // pass QImage further
  emit tileReady(jobData.x, jobData.y, jobData.z, img);
}

QgsMapSettings MapTextureGenerator::baseMapSettings()
{
  QgsMapSettings mapSettings;
  mapSettings.setLayers(project->mapLayers().values());  // TODO: correct ordering
  mapSettings.setOutputSize(QSize(res,res));
  mapSettings.setDestinationCrs(project->crs());
  return mapSettings;
}

QgsMapSettings MapTextureGenerator::mapSettingsForTile(int x, int y, int z)
{
  QgsMapSettings mapSettings(baseMapSettings());
  mapSettings.setExtent(tilingScheme.tileToExtent(x, y, z));
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
