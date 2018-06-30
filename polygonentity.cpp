#include "polygonentity.h"

#include "polygongeometry.h"
#include "map3d.h"
#include "terraingenerator.h"
#include "utils.h"

#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DCore/QTransform>

#include "qgsvectorlayer.h"
#include "qgsmultipolygon.h"



PolygonEntity::PolygonEntity(const Map3D& map, const PolygonRenderer& settings, Qt3DCore::QNode* parent)
  : Qt3DCore::QEntity(parent)
{
  QgsVectorLayer* layer = settings.layer();
  QgsPointXY origin(map.originX, map.originY);

  Qt3DExtras::QPhongMaterial* material = new Qt3DExtras::QPhongMaterial;
  material->setAmbient(settings.material.ambient());
  material->setDiffuse(settings.material.diffuse());
  material->setSpecular(settings.material.specular());
  material->setShininess(settings.material.shininess());
  addComponent(material);

  QList<QgsPolygon*> polygons;
  QgsFeature f;
  QgsFeatureRequest request;
  request.setDestinationCrs(map.crs, QgsCoordinateTransformContext());
  QgsFeatureIterator fi = layer->getFeatures(request);
  while (fi.nextFeature(f))
  {
    if (f.geometry().isNull())
      continue;

    const QgsAbstractGeometry* g = f.geometry().constGet();

    if (QgsWkbTypes::flatType(g->wkbType()) == QgsWkbTypes::Polygon)
    {
      const QgsPolygon* poly = static_cast<const QgsPolygon*>(g);
      QgsPolygon* polyClone = poly->clone();
      Utils::clampAltitudes(polyClone, settings.altClamping, settings.altBinding, settings.height, map);
      polygons.append(polyClone);
    }
    else if (QgsWkbTypes::flatType(g->wkbType()) == QgsWkbTypes::MultiPolygon)
    {
      const QgsMultiPolygon* mpoly = static_cast<const QgsMultiPolygon*>(g);
      for (int i = 0; i < mpoly->numGeometries(); ++i)
      {
        const QgsAbstractGeometry* g2 = mpoly->geometryN(i);
        Q_ASSERT(QgsWkbTypes::flatType(g2->wkbType()) == QgsWkbTypes::Polygon);
        QgsPolygon* polyClone = static_cast<const QgsPolygon*>(g2)->clone();
        Utils::clampAltitudes(polyClone, settings.altClamping, settings.altBinding, settings.height, map);
        polygons.append(polyClone);
      }
    }
    else
      qDebug() << "not a polygon";
  }

  geometry = new PolygonGeometry;
  geometry->setPolygons(polygons, origin, settings.extrusionHeight);

  Qt3DRender::QGeometryRenderer* renderer = new Qt3DRender::QGeometryRenderer;
  renderer->setGeometry(geometry);
  addComponent(renderer);

  Qt3DCore::QTransform* tform = new Qt3DCore::QTransform;
  tform->setTranslation(QVector3D(0,0,0));
  addComponent(tform);
}
