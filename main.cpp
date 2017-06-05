
#include <QApplication>

#include <Qt3DRender>
#include <Qt3DExtras>

#include "sidepanel.h"
#include "window3d.h"

#include <QBoxLayout>

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  SidePanel* sidePanel = new SidePanel;

  Window3D* view = new Window3D(sidePanel);
  QWidget *container = QWidget::createWindowContainer(view);

  QSize screenSize = view->screen()->size();
  container->setMinimumSize(QSize(200, 100));
  container->setMaximumSize(screenSize);

  QWidget widget;
  QHBoxLayout *hLayout = new QHBoxLayout(&widget);
  hLayout->setMargin(0);
  hLayout->addWidget(container, 1);
  hLayout->addWidget(sidePanel);

  widget.resize(800,600);
  widget.show();

  return app.exec();
}
