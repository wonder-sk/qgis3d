#include "pointentity.h"

#include <Qt3DRender/QAttribute>
#include <Qt3DRender/QBuffer>
#include <Qt3DRender/QEffect>
#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DRender/QGraphicsApiFilter>
#include <Qt3DRender/QMaterial>
#include <Qt3DRender/QParameter>
#include <Qt3DRender/QTechnique>

#include <Qt3DExtras/QCylinderGeometry>

#include <QUrl>
#include <QVector3D>

#include "map3d.h"

#include "qgsvectorlayer.h"
#include "qgspoint.h"


PointEntity::PointEntity(const Map3D& map, const PointRenderer& settings, Qt3DCore::QNode* parent)
  : Qt3DCore::QEntity(parent)
{
  //
  // load features
  //

  QList<QVector3D> positions;
  QgsFeature f;
  QgsFeatureRequest request;
  QgsFeatureIterator fi = settings.layer->getFeatures(request);
  while (fi.nextFeature(f))
  {
    QgsAbstractGeometry* g = f.geometry().geometry();
    if (QgsWkbTypes::flatType(g->wkbType()) == QgsWkbTypes::Point)
    {
      QgsPoint* pt = static_cast<QgsPoint*>(g);
      // TODO: use Z coordinates if the point is 3D
      positions.append(QVector3D(pt->x() - map.originX, settings.height, -(pt->y() - map.originY)));
      qDebug() << positions.last();
    }
    else
      qDebug() << "not a point";
  }

  int count = positions.count();

  QByteArray ba;
  ba.resize(count * sizeof(QVector3D));
  QVector3D *posData = reinterpret_cast<QVector3D *>(ba.data());
  for (int j = 0; j < count; ++j)
  {
    *posData = positions[j];
    ++posData;
  }

  //
  // geometry renderer
  //

  Qt3DRender::QBuffer* instanceBuffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer);
  instanceBuffer->setData(ba);

  Qt3DRender::QAttribute* instanceDataAttribute = new Qt3DRender::QAttribute;
  instanceDataAttribute->setName("pos");
  instanceDataAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
  instanceDataAttribute->setVertexBaseType(Qt3DRender::QAttribute::Float);
  instanceDataAttribute->setVertexSize(3);
  instanceDataAttribute->setDivisor(1);
  instanceDataAttribute->setBuffer(instanceBuffer);

  Qt3DExtras::QCylinderGeometry* geometry = new Qt3DExtras::QCylinderGeometry;
  geometry->setRings(2);  // how many vertices vertically
  geometry->setSlices(8); // how many vertices on circumference
  geometry->setRadius(2);
  geometry->setLength(20);
  geometry->addAttribute(instanceDataAttribute);

  Qt3DRender::QGeometryRenderer* renderer = new Qt3DRender::QGeometryRenderer;
  renderer->setGeometry(geometry);
  renderer->setInstanceCount(count);
  addComponent(renderer);

  //
  // material
  //

  Qt3DRender::QFilterKey* filterKey = new Qt3DRender::QFilterKey;
  filterKey->setName("renderingStyle");
  filterKey->setValue("forward");

  // the fragment shader implements a simplified version of phong shading that uses hardcoded light
  // (instead of whatever light we have defined in the scene)
  // TODO: use phong shading that respects lights from the scene
  Qt3DRender::QShaderProgram* shaderProgram = new Qt3DRender::QShaderProgram;
  shaderProgram->setVertexShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl("qrc:/shaders/instanced.vert")));
  shaderProgram->setFragmentShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl("qrc:/shaders/instanced.frag")));

  Qt3DRender::QRenderPass* renderPass = new Qt3DRender::QRenderPass;
  renderPass->setShaderProgram(shaderProgram);

  Qt3DRender::QTechnique* technique = new Qt3DRender::QTechnique;
  technique->addFilterKey(filterKey);
  technique->addRenderPass(renderPass);
  technique->graphicsApiFilter()->setApi(Qt3DRender::QGraphicsApiFilter::OpenGL);
  technique->graphicsApiFilter()->setProfile(Qt3DRender::QGraphicsApiFilter::CoreProfile);
  technique->graphicsApiFilter()->setMajorVersion(3);
  technique->graphicsApiFilter()->setMinorVersion(2);

  QColor clr = settings.diffuseColor;
  Qt3DRender::QParameter* param = new Qt3DRender::QParameter;
  param->setName("kdx");
  param->setValue(QVector3D(clr.red()/255.f,clr.green()/255.f,clr.blue()/255.f));

  Qt3DRender::QEffect* effect = new Qt3DRender::QEffect;
  effect->addTechnique(technique);
  effect->addParameter(param);

  Qt3DRender::QMaterial* material = new Qt3DRender::QMaterial;
  material->setEffect(effect);
  addComponent(material);
}
