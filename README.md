# QGIS 3D

This repository contains initial work to build a 3D framework for QGIS.

Work on integration into QGIS codebase has recently started and the most recent progress can be found in "3d" branch in wonder-sk/QGIS repository:
https://github.com/wonder-sk/QGIS/tree/3d

To build with 3D support, set `WITH_3D=TRUE` in CMake.

This will build `qgis_3d` library and a testing executable `qgis3d`.

Note: the testing executable `qgis3d` tries to load data from a folder specified by `dataDir` variable in src/3d/testapp/main.cpp. Please clone this repository and point `dataDir` to the path where you cloned it so it is possible to load the data files.

Pro-tip: you can specify `WITH_GUI=FALSE` to disable compilation of other bits of code and disable search for extra dependencies.

The code works with Qt >= 5.8, however it is preferred to use Qt 5.9 (LTS release). Unfortunately Qt3D in version 5.7 has some bugs that make QGIS 3D unusable.

This repository is not going to be target of further development of the 3D framework - it still contains testing data and keeps history of the initial 3D work.

![Screenshot](https://cloud.githubusercontent.com/assets/193367/24079851/25e19e74-0ccd-11e7-902c-4e9b86a9a955.png)
