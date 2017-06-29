TEMPLATE = app

QT += 3dcore 3drender 3dinput 3dextras widgets
CONFIG += c++11

SOURCES += main.cpp \
    mymesh.cpp \
    mygeometry.cpp \
    cameracontroller.cpp \
    window3d.cpp \
    sidepanel.cpp \
    maptexturegenerator.cpp \
    maptextureimage.cpp \
    quadtree.cpp \
    terrain.cpp \
    terraintile.cpp

RESOURCES += qml.qrc \
    data.qrc

QGIS_SOURCE_PATH = /home/martin/qgis/git-master
QGIS_BUILD_PATH = $${QGIS_SOURCE_PATH}/creator

INCLUDEPATH += \
  $${QGIS_SOURCE_PATH}/src/core \
  $${QGIS_SOURCE_PATH}/src/core/expression \
  $${QGIS_SOURCE_PATH}/src/core/geometry \
  $${QGIS_SOURCE_PATH}/src/core/metadata \
  $${QGIS_SOURCE_PATH}/src/core/raster \
  $${QGIS_BUILD_PATH} \
  $${QGIS_BUILD_PATH}/src/core

LIBS += -L$${QGIS_BUILD_PATH}/output/lib -lqgis_core

QT += xml

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    mymesh.h \
    mygeometry.h \
    cameracontroller.h \
    window3d.h \
    sidepanel.h \
    maptexturegenerator.h \
    maptextureimage.h \
    quadtree.h \
    terrain.h \
    terraintile.h
