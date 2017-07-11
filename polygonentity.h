#ifndef POLYGONENTITY_H
#define POLYGONENTITY_H

#include <Qt3DCore/QEntity>

class PolygonGeometry;

class QgsPointXY;
class QgsVectorLayer;

struct Map3D;
struct PolygonRenderer;

//! Entity that handles rendering of polygons
class PolygonEntity : public Qt3DCore::QEntity
{
public:
  PolygonEntity(const Map3D& map, const PolygonRenderer& settings, Qt3DCore::QNode* parent = nullptr);

  PolygonGeometry* geometry;
};

#endif // POLYGONENTITY_H
