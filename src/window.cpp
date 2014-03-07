#include <QMenuBar>
#include <QFileDialog>

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

    QAction* open_action = new QAction("Open", this);
    open_action->setShortcut(QKeySequence::Open);
    QObject::connect(open_action, SIGNAL(triggered()),
                     this, SLOT(on_open()));
    auto file_menu = menuBar()->addMenu("File");
    file_menu->addAction(open_action);

    resize(600, 400);
}

void Window::on_open()
{
    QString filename = QFileDialog::getOpenFileName(
                this, "Load .stl file", QString(), "*.stl");
    if (not filename.isNull())
    {
        load_stl(filename);
    }
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
