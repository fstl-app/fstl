#include <QApplication>

#include "window.h"
#include "mesh.h"
#include "glmesh.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Window window;
    window.show();

    window.load_stl("../../splitter/cayman.stl");
    return a.exec();
}
