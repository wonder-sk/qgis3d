
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

#include "map3d.h"

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

  Map3D map;
  map.layers << rlSat;
  map.crs = rlSat->crs();
  map.zExaggeration = 3;
  map.originZ = 0;

  map.tileTextureSize = 1024;
  //map.terrainType = Map3D::Flat;
  map.terrainType = Map3D::Dem;
  //map.terrainType = Map3D::QuantizedMesh;

  if (map.terrainType == Map3D::QuantizedMesh)
  {
    map.terrainTilingScheme = TilingScheme(QgsRectangle(-180,-90,0,90), QgsCoordinateReferenceSystem("EPSG:4326"));
  }
  else
  {
    map.terrainTilingScheme = TilingScheme(rlSat->extent(), rlSat->crs());
  }

  map.ctTerrainToMap = QgsCoordinateTransform(map.terrainTilingScheme.crs, map.crs);

  // define base terrain tile coordinates
  // TODO: when flat or from DEM, we can define it as we like
  QgsMapSettings ms;
  ms.setLayers(map.layers);
  ms.setDestinationCrs(map.terrainTilingScheme.crs);
  QgsRectangle fullExtentInTerrainCrs = ms.fullExtent();

  if (map.terrainType == Map3D::QuantizedMesh)
  {
    map.terrainTilingScheme.extentToTile(fullExtentInTerrainCrs, map.terrainBaseX, map.terrainBaseY, map.terrainBaseZ);
  }
  else
  {
    map.terrainBaseX = map.terrainBaseY = map.terrainBaseZ = 0;  // unused
  }

  // origin X,Y - at the project extent's center
  QgsPointXY centerTerrainCrs = fullExtentInTerrainCrs.center();
  QgsPointXY centerMapCrs = QgsCoordinateTransform(map.terrainTilingScheme.crs, map.crs).transform(centerTerrainCrs);
  map.originX = centerMapCrs.x();
  map.originY = centerMapCrs.y();

  map.mapGen = new MapTextureGenerator(map);

  // TODO: just for DEM terrain tiles
  map.demTerrainSize = 16;
  map.demLayer = rlDtm;
  map.tGen = new TerrainTextureGenerator(map.demLayer, map.terrainTilingScheme, map.demTerrainSize);

  SidePanel* sidePanel = new SidePanel;
  sidePanel->setMinimumWidth(150);

  Window3D* view = new Window3D(sidePanel, map);
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
