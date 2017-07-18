#ifndef TESTCHUNKLOADER_H
#define TESTCHUNKLOADER_H

#include "chunkloader.h"


class TestChunkLoader : public ChunkLoader
{
public:
  TestChunkLoader(ChunkNode* node);

  virtual void load() override;

  virtual Qt3DCore::QEntity *createEntity(Qt3DCore::QEntity* parent) override;
};


class TestChunkLoaderFactory : public ChunkLoaderFactory
{
public:
  virtual ChunkLoader *createChunkLoader(ChunkNode* node) const override;
};

#endif // TESTCHUNKLOADER_H
