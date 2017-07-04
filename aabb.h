#ifndef AABB_H
#define AABB_H

#include <math.h>
#include <QVector3D>

//! axis-aligned bounding box - in world coords
class AABB
{
public:
  AABB()
    : xMin(0), yMin(0), zMin(0), xMax(0), yMax(0), zMax(0)
  {
  }

  AABB(float xMin, float yMin, float zMin, float xMax, float yMax, float zMax)
    : xMin(xMin), yMin(yMin), zMin(zMin), xMax(xMax), yMax(yMax), zMax(zMax)
  {
  }

  float distanceFromPoint(float x, float y, float z) const
  {
    float dx = qMax(xMin - x, qMax(0.f, x - xMax));
    float dy = qMax(yMin - y, qMax(0.f, y - yMax));
    float dz = qMax(zMin - z, qMax(0.f, z - zMax));
    return sqrt(dx*dx + dy*dy + dz*dz);
  }

  float distanceFromPoint(const QVector3D& v) const
  {
    return distanceFromPoint(v.x(), v.y(), v.z());
  }

  QList<QVector3D> verticesForLines() const
  {
    QList<QVector3D> vertices;
    for (int i = 0; i < 2; ++i)
    {
      float x = i ? xMax : xMin;
      for (int j = 0; j < 2; ++j)
      {
        float y = j ? yMax : yMin;
        for (int k = 0; k < 2; ++k)
        {
          float z = k ? zMax : zMin;
          if (i == 0)
          {
            vertices.append(QVector3D(xMin, y, z));
            vertices.append(QVector3D(xMax, y, z));
          }
          if (j == 0)
          {
            vertices.append(QVector3D(x, yMin, z));
            vertices.append(QVector3D(x, yMax, z));
          }
          if (k == 0)
          {
            vertices.append(QVector3D(x, y, zMin));
            vertices.append(QVector3D(x, y, zMax));
          }
        }
      }
    }
    return vertices;
  }

  float xMin, yMin, zMin;
  float xMax, yMax, zMax;
};

#endif // AABB_H
