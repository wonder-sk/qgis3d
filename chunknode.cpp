#include "chunknode.h"

#include "chunkedentity.h"  // for ChunkLoader destructor
#include "chunklist.h"
#include "chunkloader.h"
#include <Qt3DCore/QEntity>


ChunkNode::ChunkNode(const AABB &bbox, float error)
  : bbox(bbox)
  , error(error)
  , parent(nullptr)
  , state(Skeleton)
  , loaderQueueEntry(nullptr)
  , replacementQueueEntry(nullptr)
  , loader(nullptr)
  , entity(nullptr)
{
  for (int i = 0; i < 4; ++i)
    children[i] = nullptr;
}

ChunkNode::~ChunkNode()
{
  Q_ASSERT(!loaderQueueEntry);
  Q_ASSERT(!replacementQueueEntry);
  Q_ASSERT(!loader);   // should be deleted when removed from loader queue
  Q_ASSERT(!entity);   // should be deleted when removed from replacement queue
  Q_ASSERT(state == Skeleton);
  for (int i = 0; i < 4; ++i)
    delete children[i];
}

bool ChunkNode::allChildChunksResident(const QTime& currentTime) const
{
  for (int i = 0; i < 4; ++i)
  {
    if (!children[i])
      return false;  // not even a skeleton
    if (children[i]->state != Loaded)
      return false;  // no there yet
    if (children[i]->entityCreatedTime.msecsTo(currentTime) < 100)
      return false;  // allow some time for upload of stuff within Qt3D (TODO: better way to check it is ready?)
  }
  return true;
}

void ChunkNode::ensureAllChildrenExist()
{
  for (int i = 0; i < 4; ++i)
  {
    if (!children[i])
    {
      float xmin, ymin, zmin, xmax, ymax, zmax;
      float xc = bbox.xCenter(), zc = bbox.zCenter();
      ymin = bbox.yMin;
      ymax = bbox.yMax;
      if (i == 0 || i == 2)  // lower
      {
        zmin = bbox.zMin;
        zmax = zc;
      }
      else
      {
        zmin = zc;
        zmax = bbox.zMax;
      }
      if (i == 0 || i == 1)  // left
      {
        xmin = bbox.xMin;
        xmax = xc;
      }
      else
      {
        xmin = xc;
        xmax = bbox.xMax;
      }
      children[i] = new ChunkNode(AABB(xmin, ymin, zmin, xmax, ymax, zmax), error/2);
      children[i]->parent = this;
    }
  }
}

int ChunkNode::level() const
{
  int lvl = 0;
  ChunkNode* p = parent;
  while (p)
  {
    ++lvl;
    p = p->parent;
  }
  return lvl;
}

void ChunkNode::setLoading(ChunkLoader *chunkLoader, ChunkListEntry *entry)
{
  Q_ASSERT(!loaderQueueEntry);
  Q_ASSERT(!loader);

  state = ChunkNode::Loading;
  loader = chunkLoader;
  loaderQueueEntry = entry;
}

void ChunkNode::setLoaded(Qt3DCore::QEntity *newEntity, ChunkListEntry *entry)
{
  Q_ASSERT(state == ChunkNode::Loading);
  Q_ASSERT(loader);

  entity = newEntity;
  entityCreatedTime = QTime::currentTime();

  delete loader;
  loader = nullptr;

  state = ChunkNode::Loaded;
  loaderQueueEntry = nullptr;
  replacementQueueEntry = entry;
}

void ChunkNode::unloadChunk()
{
  Q_ASSERT(state == ChunkNode::Loaded);
  Q_ASSERT(entity);
  Q_ASSERT(replacementQueueEntry);

  entity->deleteLater();
  entity = nullptr;
  delete replacementQueueEntry;
  replacementQueueEntry = nullptr;
  state = ChunkNode::Skeleton;
}
