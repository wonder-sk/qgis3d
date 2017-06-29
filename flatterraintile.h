#ifndef FLATTERRAINTILE_H
#define FLATTERRAINTILE_H

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DExtras/QPlaneGeometry>
#include <Qt3DExtras/QDiffuseMapMaterial>

class MapTextureGenerator;
struct QuadTreeNode;
class FlatTerrainTileMesh;

class FlatTerrainTile : public Qt3DCore::QEntity
{
public:
  FlatTerrainTile(QuadTreeNode* node, Qt3DExtras::QPlaneGeometry* tileGeometry, MapTextureGenerator* mapGen, Qt3DCore::QNode *parent = nullptr);

#if 0
  void setY(float y)  // to avoid z-fighting
  {
    QVector3D tr = transform->translation();
    tr.setY(y);
    transform->setTranslation(tr);
  }
#endif

private:
  FlatTerrainTileMesh* mesh;
  Qt3DExtras::QDiffuseMapMaterial* material;
  //Qt3DExtras::QPhongMaterial* material;
  Qt3DCore::QTransform* transform;
};

#endif // FLATTERRAINTILE_H
