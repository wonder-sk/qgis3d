
#include <QGuiApplication>

#include <Qt3DRender>
#include <Qt3DExtras>

#include "window3d.h"


int main(int argc, char *argv[])
{
  QGuiApplication app(argc, argv);

  Window3D view;
  view.resize(800/2,600/2);
  view.show();

  return app.exec();
}
