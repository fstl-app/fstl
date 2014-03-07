#include <QMenuBar>
#include <QMessageBox>
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

    about_action = new QAction("About", this);
    QObject::connect(about_action, SIGNAL(triggered()),
                     this, SLOT(on_about()));

    auto file_menu = menuBar()->addMenu("File");
    file_menu->addAction(open_action);
    file_menu->addAction(quit_action);

    auto help_menu = menuBar()->addMenu("Help");
    help_menu->addAction(about_action);

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

void Window::on_about()
{
    QMessageBox::about(this, "About fstl", "<b>fstl</b><br><br>"
                       "A fast viewer for <code>.stl</code> files.<br>"
                       "<a href=\"https://github.com/mkeeter/fstl\">https://github.com/mkeeter/fstl</a><br><br>"
                       "© 2014 Matthew Keeter<br>"
                       "<a href=\"mailto:matt.j.keeter@gmail.com\">matt.j.keeter@gmail.com</a><br>");
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
    Loader* loader = new Loader(this, filename);
    connect(loader, SIGNAL(started()),
            this, SLOT(disable_open_action()));
    connect(loader, SIGNAL(got_mesh(Mesh*)),
            canvas, SLOT(load_mesh(Mesh*)));
    connect(loader, SIGNAL(finished()),
            loader, SLOT(deleteLater()));
    connect(loader, SIGNAL(finished()),
            this, SLOT(enable_open_action()));
    connect(loader, SIGNAL(loaded_file(QString)),
            this, SLOT(setWindowTitle(QString)));
    loader->start();
}
