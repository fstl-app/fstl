#include "window.h"
#include "canvas.h"
#include "loader.h"

Window::Window(QWidget *parent) :
    QMainWindow(parent)
{
    setWindowTitle("fstl");

    QGLFormat format;
    format.setVersion(2, 1);
    format.setSampleBuffers(true);

    canvas = new Canvas(format, this);
    setCentralWidget(canvas);
}

void Window::load_stl(const QString &filename)
{
    Loader* loader = new Loader(this, filename);
    connect(loader, SIGNAL(got_mesh(Mesh*)),
            canvas, SLOT(load_mesh(Mesh*)));
    connect(loader, SIGNAL(finished()),
            loader, SLOT(deleteLater()));
    loader->start();
}
