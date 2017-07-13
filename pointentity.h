#ifndef POINTENTITY_H
#define POINTENTITY_H

#include <Qt3DCore/QEntity>

struct Map3D;
struct PointRenderer;

class PointEntity : public Qt3DCore::QEntity
{
public:
  PointEntity(const Map3D& map, const PointRenderer& settings, Qt3DCore::QNode* parent = nullptr);
};

#endif // POINTENTITY_H
