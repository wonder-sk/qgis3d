
#include <Qt3DQuickExtras/qt3dquickwindow.h>
#include <Qt3DQuick/QQmlAspectEngine>
#include <QGuiApplication>
#include <QQmlContext>

#include "mymesh.h"

int main(int argc, char *argv[])
{
  QGuiApplication app(argc, argv);

  qmlRegisterType<MyMesh>("qgis3d", 1, 0, "MyMesh");

  Qt3DExtras::Quick::Qt3DQuickWindow view;
  view.resize(1600, 800);
  view.engine()->qmlEngine()->rootContext()->setContextProperty("_window", &view);
  view.setSource(QUrl("qrc:/main.qml"));
  view.show();

  //QQmlApplicationEngine engine;
  //engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

  return app.exec();
}
