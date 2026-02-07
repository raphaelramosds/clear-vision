# CVISION GUI

## Overview

Graphic User Interface for demonstrating the cvision library

## Set up

### MSYS2

First of all, install [MSYS 2](https://www.mingw-w64.org/getting-started/msys2/)

Install Qt 6

```bash
pacman -S mingw-w64-x86_64-cmake \
    mingw-w64-x86_64-opencv \
    mingw-w64-x86_64-qt6 \
    mingw-w64-x86_64-ffmpeg \
    mingw-w64-x86_64-gstreamer
```

On Projects, set the following environment variables on Qt project, with **Batch Edit**

```bash
-DCMAKE_C_COMPILER:FILEPATH=C:/msys64/mingw64/bin/gcc.exe
-DCMAKE_PREFIX_PATH:PATH=C:\Qt\6.10.2\mingw_64;C:\msys64\mingw64
-DQT_DIR:PATH=C:/msys64/mingw64/lib/cmake/Qt6
-DQT_MAINTENANCE_TOOL:FILEPATH=C:/Qt/MaintenanceTool.exe
-DQT_QMAKE_EXECUTABLE:FILEPATH=C:/msys64/mingw64/bin/qmake.exe
-DQt6Concurrent_DIR:PATH=C:/msys64/mingw64/lib/cmake/Qt6Concurrent
-DQt6CoreTools_DIR:PATH=C:/msys64/mingw64/lib/cmake/Qt6CoreTools
-DQt6Core_DIR:PATH=C:/msys64/mingw64/lib/cmake/Qt6Core
-DQt6EntryPointPrivate_DIR:PATH=C:/msys64/mingw64/lib/cmake/Qt6EntryPointPrivate
-DQt6GuiTools_DIR:PATH=C:/msys64/mingw64/lib/cmake/Qt6GuiTools
-DQt6Gui_DIR:PATH=C:/msys64/mingw64/lib/cmake/Qt6Gui
-DQt6MultimediaWidgets_DIR:PATH=C:/msys64/mingw64/lib/cmake/Qt6MultimediaWidgets
-DQt6Multimedia_DIR:PATH=C:/msys64/mingw64/lib/cmake/Qt6Multimedia
-DQt6Network_DIR:PATH=C:/msys64/mingw64/lib/cmake/Qt6Network
-DQt6WidgetsTools_DIR:PATH=C:/msys64/mingw64/lib/cmake/Qt6WidgetsTools
-DQt6Widgets_DIR:PATH=C:/msys64/mingw64/lib/cmake/Qt6Widgets
-DQt6_DIR:PATH=C:/msys64/mingw64/lib/cmake/Qt6
-DWINDEPLOYQT_EXECUTABLE:FILEPATH=C:/msys64/mingw64/bin/windeployqt.exe
``
