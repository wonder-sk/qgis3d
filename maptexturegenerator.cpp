#include "maptexturegenerator.h"

#include <qgsmaprenderersequentialjob.h>
#include <qgsmapsettings.h>
#include <qgsproject.h>

MapTextureGenerator::MapTextureGenerator(QgsProject* project, const TilingScheme& tilingScheme)
  : project(project)
  , tilingScheme(tilingScheme)
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
  mapSettings.setOutputSize(QSize(512,512));
  mapSettings.setDestinationCrs(project->crs());
  return mapSettings;
}

QgsMapSettings MapTextureGenerator::mapSettingsForTile(int x, int y, int z)
{
  QgsPointXY pt0 = tilingScheme.tileToMap(x, y, z);
  QgsPointXY pt1 = tilingScheme.tileToMap(x+1, y+1, z);

  QgsMapSettings mapSettings(baseMapSettings());
  mapSettings.setExtent(QgsRectangle(pt0, pt1));
  return mapSettings;
}
