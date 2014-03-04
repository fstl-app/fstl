QT       += core gui opengl widgets

TARGET = fstl
TEMPLATE = app

SOURCES += \
    main.cpp\
    canvas.cpp \
    mesh.cpp \
    glmesh.cpp \
    loader.cpp \
    window.cpp

HEADERS  += \
    canvas.h \
    mesh.h \
    glmesh.h \
    loader.h \
    window.h

CONFIG += c++11

INCLUDEPATH += /usr/local/include/eigen3

RESOURCES += \
    resources.qrc
