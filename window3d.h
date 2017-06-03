#ifndef WINDOW3D_H
#define WINDOW3D_H

#include <Qt3DExtras>

class CameraController;

class Window3D : public Qt3DExtras::Qt3DWindow
{
public:
  Window3D();

  Qt3DCore::QEntity *createScene();

protected:
  void resizeEvent(QResizeEvent *ev) override;

private:
  CameraController* cc;
};

#endif // WINDOW3D_H
