#ifndef CHUNKNODE_H
#define CHUNKNODE_H


#include "aabb.h"

#include <QTime>

namespace Qt3DCore
{
  class QEntity;
}

class ChunkListEntry;
class ChunkLoader;

class ChunkNode
{
public:
  //! constructs a skeleton chunk
  ChunkNode(int x, int y, int z, const AABB& bbox, float error, ChunkNode* parent = nullptr);

  ~ChunkNode();

  bool allChildChunksResident(const QTime& currentTime) const;

  //! make sure that all child nodes are at least skeleton nodes
  void ensureAllChildrenExist();

  int level() const;

  //! mark a chunk as being loaded, using the passed loader
  void setLoading(ChunkLoader* chunkLoader, ChunkListEntry* entry);

  //! mark a chunk as loaded, using the loaded entity
  void setLoaded(Qt3DCore::QEntity* entity, ChunkListEntry* entry);

  //! turn a loaded chunk into skeleton
  void unloadChunk();

  //! called when bounding box
  void setExactBbox(const AABB& box);

  AABB bbox;      //!< bounding box in world coordinates
  float error;    //!< error of the node in world coordinates

  int x,y,z;    //!< chunk coordinates (for use with a tiling scheme)

  ChunkNode* parent;        //!< TODO: should be shared pointer
  ChunkNode* children[4];   //!< TODO: should be weak pointers. May be null if not created yet or removed already

  enum State
  {
    Skeleton,   //!< does not contain data of the chunk and data are not being loaded
    Loading,    //!< data are not available yet, but node is in the request queue
    Loaded,     //!< data are fully available
  };

  State state;  //!< state of the node

  ChunkListEntry* loaderQueueEntry;  //!< not null <=> Loading state
  ChunkListEntry* replacementQueueEntry;  //!< not null <=> Loaded state

  ChunkLoader* loader;         //!< contains extra data necessary for entity creation (not null <=> Loading state)
  Qt3DCore::QEntity* entity;   //!< contains everything to display chunk as 3D object (not null <=> Loaded state)

  QTime entityCreatedTime;
};

#endif // CHUNKNODE_H
