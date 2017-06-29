
#include <QApplication>

#include <Qt3DRender>
#include <Qt3DExtras>

#include "maptexturegenerator.h"
#include "sidepanel.h"
#include "window3d.h"

#include <qgsapplication.h>
#include <qgsmapsettings.h>
#include <qgsrasterlayer.h>
#include <qgsproject.h>

#include <QBoxLayout>

int main(int argc, char *argv[])
{
  // TODO: why it does not work to create ordinary QApplication and then just call initQgis()

  qputenv("QGIS_PREFIX_PATH", "/home/martin/qgis/git-master/creator/output");
  QgsApplication app(argc, argv, true);
  QgsApplication::initQgis();

  QgsRasterLayer* rlDtm = new QgsRasterLayer("/home/martin/tmp/qgis3d/dtm.tif", "dtm", "gdal");
  Q_ASSERT( rlDtm->isValid() );

  QgsRasterLayer* rlSat = new QgsRasterLayer("/home/martin/tmp/qgis3d/ap.tif", "ap", "gdal");
  Q_ASSERT( rlSat->isValid() );

  QgsProject* prj = new QgsProject();
  prj->addMapLayer(rlSat);
  prj->setCrs(rlSat->crs());

  // determine tiling scheme for this project
  QgsMapSettings ms;
  ms.setLayers(prj->mapLayers().values());
  ms.setDestinationCrs(prj->crs());
  TilingScheme tilingScheme(ms.fullExtent());

  MapTextureGenerator* mapGen = new MapTextureGenerator(prj, tilingScheme);
  //TerrainGenerator tGen(rlDtm);

  SidePanel* sidePanel = new SidePanel;
  sidePanel->setMinimumWidth(150);

  Window3D* view = new Window3D(sidePanel, mapGen);
  QWidget *container = QWidget::createWindowContainer(view);

  QSize screenSize = view->screen()->size();
  container->setMinimumSize(QSize(200, 100));
  container->setMaximumSize(screenSize);

  QWidget widget;
  QHBoxLayout *hLayout = new QHBoxLayout(&widget);
  hLayout->setMargin(0);
  hLayout->addWidget(container, 1);
  hLayout->addWidget(sidePanel);

  widget.resize(800,600);
  widget.show();

  return app.exec();
}
