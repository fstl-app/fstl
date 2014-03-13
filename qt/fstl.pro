QT       += core gui opengl widgets

TARGET = fstl
TEMPLATE = app

SOURCES += \
    ../src/main.cpp\
    ../src/canvas.cpp \
    ../src/mesh.cpp \
    ../src/glmesh.cpp \
    ../src/loader.cpp \
    ../src/window.cpp \
    ../src/backdrop.cpp

HEADERS  += \
    ../src/canvas.h \
    ../src/mesh.h \
    ../src/glmesh.h \
    ../src/loader.h \
    ../src/window.h \
    ../src/backdrop.h

CONFIG += c++11

RESOURCES += \
    qt.qrc \
    ../gl/gl.qrc

QMAKE_INFO_PLIST = ../misc/Info.plist

ICON = ../misc/fstl.icns
