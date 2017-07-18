#ifndef CHUNKEDENTITY_H
#define CHUNKEDENTITY_H

#include <Qt3DCore/QEntity>

class AABB;
class ChunkNode;
class ChunkList;
class ChunkLoaderFactory;


#include <QVector3D>
#include <QMatrix4x4>
//! Records some bits about the scene
class SceneState
{
public:
  QVector3D cameraPos;
  float cameraFov;
  int screenSizePx;

  QMatrix4x4 viewProjectionMatrix; //!< for frustum culling
};

#include <QTime>

//! Implementation of entity that handles chunks of data organized in quadtree with loading data when necessary
//! based on data error and unloading of data when data are not necessary anymore
class ChunkedEntity : public Qt3DCore::QEntity
{
public:
  ChunkedEntity(const AABB& rootBbox, float rootError, float tau, int maxLevel, ChunkLoaderFactory* loaderFactory, Qt3DCore::QNode* parent = nullptr);
  ~ChunkedEntity();

  //!< called when e.g. camera changes and entity may need updated
  void update(const SceneState& state);

  bool needsUpdate; //!< a chunk has been loaded recently - let's display it!

private:
  void update(ChunkNode* node, const SceneState& state);

  //! make sure that the chunk will be loaded soon (if not loaded yet) and not unloaded anytime soon (if loaded already)
  void requestResidency(ChunkNode* node);

private:
  //! root node of the quadtree hierarchy
  ChunkNode* rootNode;
  //! max. allowed screen space error
  float tau;
  //! maximum allowed depth of quad tree
  int maxLevel;
  //! factory that creates loaders for individual chunk nodes
  ChunkLoaderFactory* chunkLoaderFactory;
  //! queue of chunks to be loaded
  ChunkList* chunkLoaderQueue;
  //! queue of chunk to be eventually replaced
  ChunkList* replacementQueue;

  QList<ChunkNode*> activeNodes;
  int frustumCulled;

  // TODO: max. length for loading queue

  QTime currentTime;

  //! max. length for replacement queue
  int maxLoadedChunks;
};


#endif // CHUNKEDENTITY_H
