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

    open_action = new QAction("Open", this);
    open_action->setShortcut(QKeySequence::Open);
    QObject::connect(open_action, SIGNAL(triggered()),
                     this, SLOT(on_open()));

    quit_action = new QAction("Quit", this);
    quit_action->setShortcut(QKeySequence::Quit);
    QObject::connect(quit_action, SIGNAL(triggered()),
                     this, SLOT(close()));

    auto file_menu = menuBar()->addMenu("File");
    file_menu->addAction(open_action);
    file_menu->addAction(quit_action);

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


void Window::enable_open_action()
{
    open_action->setEnabled(true);
}


void Window::disable_open_action()
{

    open_action->setEnabled(false);
}


void Window::load_stl(const QString &filename)
{
    disable_open_action();
    Loader* loader = new Loader(this, filename);
    connect(loader, SIGNAL(got_mesh(Mesh*)),
            canvas, SLOT(load_mesh(Mesh*)));
    connect(loader, SIGNAL(finished()),
            loader, SLOT(deleteLater()));
    connect(loader, SIGNAL(finished()),
            this, SLOT(enable_open_action()));
    loader->start();
}
