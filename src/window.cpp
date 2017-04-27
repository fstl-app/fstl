#include <QMenuBar>
#include <QMessageBox>
#include <QFileDialog>

#include "window.h"
#include "canvas.h"
#include "loader.h"

Window::Window(QWidget *parent) :
    QMainWindow(parent),
    open_action(new QAction("Open", this)),
    about_action(new QAction("About", this)),
    quit_action(new QAction("Quit", this)),
    perspective_action(new QAction("Perspective", this)),
    orthogonal_action(new QAction("Orthogonal", this)),
    watcher(new QFileSystemWatcher(this))

{
    setWindowTitle("fstl");
    setAcceptDrops(true);

    QFile styleFile(":/qt/style.qss");
    styleFile.open( QFile::ReadOnly );
    setStyleSheet(styleFile.readAll());

    QGLFormat format;
    format.setVersion(2, 1);
    format.setSampleBuffers(true);

    canvas = new Canvas(format, this);
    setCentralWidget(canvas);

    QObject::connect(watcher, &QFileSystemWatcher::fileChanged,
                     this, &Window::on_watched_change);

    open_action->setShortcut(QKeySequence::Open);
    QObject::connect(open_action, &QAction::triggered,
                     this, &Window::on_open);

    quit_action->setShortcut(QKeySequence::Quit);
    QObject::connect(quit_action, &QAction::triggered,
                     this, &Window::close);

    QObject::connect(about_action, &QAction::triggered,
                     this, &Window::on_about);

    auto file_menu = menuBar()->addMenu("File");
    file_menu->addAction(open_action);
    file_menu->addAction(quit_action);

    auto help_menu = menuBar()->addMenu("Help");
    help_menu->addAction(about_action);

    auto view_menu = menuBar()->addMenu("View");
    auto projection_menu = view_menu->addMenu("Projection");
    projection_menu->addAction(perspective_action);
    projection_menu->addAction(orthogonal_action);
    auto projections = new QActionGroup(projection_menu);
    for (auto p : {perspective_action, orthogonal_action})
    {
        projections->addAction(p);
        p->setCheckable(true);
    }
    perspective_action->setChecked(true);
    projections->setExclusive(true);
    QObject::connect(projections, &QActionGroup::triggered,
                     this, &Window::on_projection);

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
    QMessageBox::about(this, "",
        "<p align=\"center\"><b>fstl</b></p>"
        "<p>A fast viewer for <code>.stl</code> files.<br>"
        "<a href=\"https://github.com/mkeeter/fstl\""
        "   style=\"color: #93a1a1;\">https://github.com/mkeeter/fstl</a></p>"
        "<p>© 2014 Matthew Keeter<br>"
        "<a href=\"mailto:matt.j.keeter@gmail.com\""
        "   style=\"color: #93a1a1;\">matt.j.keeter@gmail.com</a></p>");
}

void Window::on_bad_stl()
{
    QMessageBox::critical(this, "Error",
                          "<b>Error:</b><br>"
                          "This <code>.stl</code> file is invalid or corrupted.<br>"
                          "Please export it from the original source, verify, and retry.");
}

void Window::on_missing_file()
{
    QMessageBox::critical(this, "Error",
                          "<b>Error:</b><br>"
                          "The target file is missing.<br>");
}

void Window::enable_open()
{
    open_action->setEnabled(true);
}

void Window::disable_open()
{
    open_action->setEnabled(false);
}

void Window::set_watched(const QString& filename)
{
    const auto files = watcher->files();
    if (files.size())
    {
        watcher->removePaths(watcher->files());
    }
    watcher->addPath(filename);
}

void Window::on_projection(QAction* proj)
{
    canvas->set_perspective(proj == perspective_action);
}

void Window::on_watched_change(const QString& filename)
{
    load_stl(filename);
}

bool Window::load_stl(const QString& filename)
{
    if (!open_action->isEnabled())  return false;

    canvas->set_status("Loading " + filename);

    Loader* loader = new Loader(this, filename);
    connect(loader, &Loader::started,
              this, &Window::disable_open);

    connect(loader, &Loader::got_mesh,
            canvas, &Canvas::load_mesh);
    connect(loader, &Loader::error_bad_stl,
              this, &Window::on_bad_stl);
    connect(loader, &Loader::error_missing_file,
              this, &Window::on_missing_file);

    connect(loader, &Loader::finished,
            loader, &Loader::deleteLater);
    connect(loader, &Loader::finished,
              this, &Window::enable_open);
    connect(loader, &Loader::finished,
            canvas, &Canvas::clear_status);

    if (filename[0] != ':')
    {
        connect(loader, &Loader::loaded_file,
                  this, &Window::setWindowTitle);
        connect(loader, &Loader::loaded_file,
                  this, &Window::set_watched);
    }

    loader->start();
    return true;
}

void Window::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls())
    {
        auto urls = event->mimeData()->urls();
        if (urls.size() == 1 && urls.front().path().endsWith(".stl"))
            event->acceptProposedAction();
    }
}

void Window::dropEvent(QDropEvent *event)
{
    load_stl(event->mimeData()->urls().front().toLocalFile());
}
