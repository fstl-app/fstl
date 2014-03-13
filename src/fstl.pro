QT       += core gui opengl widgets

TARGET = fstl
TEMPLATE = app

SOURCES += \
    main.cpp\
    canvas.cpp \
    mesh.cpp \
    glmesh.cpp \
    loader.cpp \
    window.cpp \
    backdrop.cpp

HEADERS  += \
    canvas.h \
    mesh.h \
    glmesh.h \
    loader.h \
    window.h \
    backdrop.h

CONFIG += c++11

RESOURCES += \
    resources.qrc

QMAKE_INFO_PLIST = ../misc/Info.plist

ICON = ../misc/fstl.icns
