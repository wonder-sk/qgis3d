#include "testchunkloader.h"

#include "chunkedentity.h"
#include "chunknode.h"

#include <Qt3DCore/QTransform>
#include <Qt3DExtras/QCuboidMesh>
#include <Qt3DExtras/QPhongMaterial>


static QColor tileColors[] = {
  Qt::green,
  Qt::red,
  Qt::blue,
  Qt::cyan,
  Qt::magenta,
  Qt::yellow,
};
static int tileColorsCount = sizeof(tileColors) / sizeof(QColor);


ChunkLoader *TestChunkLoaderFactory::createChunkLoader(ChunkNode *node) const
{
  return new TestChunkLoader(node);
}



TestChunkLoader::TestChunkLoader(ChunkNode *node)
  : ChunkLoader(node)
{
}


void TestChunkLoader::load()
{
  // nothing to do here really...
}


Qt3DCore::QEntity *TestChunkLoader::createEntity(Qt3DCore::QEntity *parent)
{
  Qt3DCore::QEntity* entity = new Qt3DCore::QEntity;

  Qt3DExtras::QCuboidMesh* mesh = new Qt3DExtras::QCuboidMesh;
  mesh->setXExtent(node->bbox.xExtent()*.9);
  mesh->setYExtent(node->bbox.yExtent()*.9);
  mesh->setZExtent(node->bbox.zExtent()*.9);
  entity->addComponent(mesh);

  Qt3DExtras::QPhongMaterial* material = new Qt3DExtras::QPhongMaterial;
  QColor color = tileColors[node->level() % tileColorsCount];
  material->setDiffuse(color);
  entity->addComponent(material);

  Qt3DCore::QTransform* transform = new Qt3DCore::QTransform;
  transform->setTranslation(node->bbox.center());
  entity->addComponent(transform);

  entity->setEnabled(false);
  entity->setParent(parent);
  return entity;
}
