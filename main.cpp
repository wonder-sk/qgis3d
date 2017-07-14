
#include <QApplication>
#include <QBoxLayout>

#include <Qt3DRender>
#include <Qt3DExtras>

#include "maptexturegenerator.h"
#include "sidepanel.h"
#include "window3d.h"
#include "map3d.h"
#include "flatterraingenerator.h"
#include "demterraingenerator.h"
#include "quantizedmeshterraingenerator.h"

#include <qgsapplication.h>
#include <qgsmapsettings.h>
#include <qgsrasterlayer.h>
#include <qgsproject.h>
#include <qgsvectorlayer.h>



static QgsRectangle _fullExtent(const QList<QgsMapLayer*>& layers, const QgsCoordinateReferenceSystem& crs)
{
  QgsMapSettings ms;
  ms.setLayers(layers);
  ms.setDestinationCrs(crs);
  return ms.fullExtent();
}

int main(int argc, char *argv[])
{
  // TODO: why it does not work to create ordinary QApplication and then just call initQgis()

  qputenv("QGIS_PREFIX_PATH", "/home/martin/qgis/git-master/build59/output");
  QgsApplication app(argc, argv, true);
  QgsApplication::initQgis();

  QgsRasterLayer* rlDtm = new QgsRasterLayer("/home/martin/tmp/qgis3d/dtm.tif", "dtm", "gdal");
  Q_ASSERT( rlDtm->isValid() );

  QgsRasterLayer* rlSat = new QgsRasterLayer("/home/martin/tmp/qgis3d/ap.tif", "ap", "gdal");
  Q_ASSERT( rlSat->isValid() );

  QgsVectorLayer* vlPolygons = new QgsVectorLayer("/home/martin/tmp/qgis3d/data/buildings.shp", "buildings", "ogr");
  Q_ASSERT( vlPolygons->isValid() );

  QgsVectorLayer* vlPoints = new QgsVectorLayer("/home/martin/tmp/qgis3d/data/trees.shp", "trees", "ogr");
  Q_ASSERT( vlPoints->isValid() );

  Map3D map;
  map.layers << rlSat;
  map.crs = rlSat->crs();
  map.zExaggeration = 3;

  TerrainGenerator::Type tt;
  tt = TerrainGenerator::Flat;
  //tt = TerrainGenerator::Dem;
  //tt = TerrainGenerator::QuantizedMesh;

  if (tt == TerrainGenerator::Flat)
  {
    // TODO: tiling scheme - from this project's CRS + full extent
    FlatTerrainGenerator* flatTerrain = new FlatTerrainGenerator;
    map.terrainGenerator.reset(flatTerrain);
  }
  else if (tt == TerrainGenerator::Dem)
  {
    DemTerrainGenerator* demTerrain = new DemTerrainGenerator(rlDtm, 16);
    map.terrainGenerator.reset(demTerrain);
  }
  else if (tt == TerrainGenerator::QuantizedMesh)
  {
    QuantizedMeshTerrainGenerator* qmTerrain = new QuantizedMeshTerrainGenerator;
    map.terrainGenerator.reset(qmTerrain);
  }

  Q_ASSERT(map.terrainGenerator);  // we need a terrain generator

  if (map.terrainGenerator->type() == TerrainGenerator::Flat)
  {
    // we are free to define terrain extent to whatever works best
    static_cast<FlatTerrainGenerator*>(map.terrainGenerator.get())->setExtent(_fullExtent(map.layers, map.crs), map.crs);
  }

  QgsRectangle fullExtentInTerrainCrs = _fullExtent(map.layers, map.terrainGenerator->crs());

  if (map.terrainGenerator->type() == TerrainGenerator::QuantizedMesh)
  {
    // define base terrain tile coordinates
    static_cast<QuantizedMeshTerrainGenerator*>(map.terrainGenerator.get())->setBaseTileFromExtent(fullExtentInTerrainCrs);
  }

  // origin X,Y - at the project extent's center
  QgsPointXY centerTerrainCrs = fullExtentInTerrainCrs.center();
  QgsPointXY centerMapCrs = QgsCoordinateTransform(map.terrainGenerator->terrainTilingScheme.crs, map.crs).transform(centerTerrainCrs);
  map.originX = centerMapCrs.x();
  map.originY = centerMapCrs.y();

  // polygons

  PolygonRenderer pr;
  pr.layer = vlPolygons;
  pr.ambientColor = Qt::gray;
  pr.diffuseColor = Qt::lightGray;
  pr.height = 0;
  pr.extrusionHeight = 10;
  map.polygonRenderers << pr;

  // points

  PointRenderer ptr;
  ptr.layer = vlPoints;
  ptr.diffuseColor = QColor(222,184,135);
  ptr.height = 5;
  ptr.shapeProperties["shape"] = "cylinder";
  ptr.shapeProperties["radius"] = 2;
  ptr.shapeProperties["length"] = 10;
  //Qt3DCore::QTransform tr;
  //tr.setScale3D(QVector3D(4,1,4));
  //ptr.transform = tr.matrix();
  map.pointRenderers << ptr;

  PointRenderer ptr2;
  ptr2.layer = vlPoints;
  ptr2.diffuseColor = QColor(60,179,113);
  ptr2.height = 15;
  ptr2.shapeProperties["shape"] = "sphere";
  ptr2.shapeProperties["radius"] = 7;
  map.pointRenderers << ptr2;

#if 0
  // Q on top of trees - only in Qt 5.9
  PointRenderer ptr3;
  ptr3.layer = vlPoints;
  ptr3.diffuseColor = QColor(88, 150, 50);
  ptr3.height = 25;
  ptr3.shapeProperties["shape"] = "extrudedText";
  ptr3.shapeProperties["text"] = "Q";
  Qt3DCore::QTransform tr;
  tr.setScale3D(QVector3D(3,3,3));
  tr.setTranslation(QVector3D(-4,0,0));
  ptr3.transform = tr.matrix();
  map.pointRenderers << ptr3;
#endif

  // skybox

  map.skybox = true;
  map.skyboxFileBase = "file:///home/martin/tmp/qgis3d/skybox/miramar";
  map.skyboxFileExtension = ".jpg";

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
