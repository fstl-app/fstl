#include <QMenuBar>
#include <QMessageBox>
#include <QFileDialog>

#include "window.h"
#include "canvas.h"
#include "loader.h"

const QString Window::RECENT_FILE_KEY = "recentFiles";

Window::Window(QWidget *parent) :
    QMainWindow(parent),
    open_action(new QAction("Open", this)),
    about_action(new QAction("About", this)),
    quit_action(new QAction("Quit", this)),
    perspective_action(new QAction("Perspective", this)),
    orthogonal_action(new QAction("Orthographic", this)),
    reload_action(new QAction("Reload", this)),
    autoreload_action(new QAction("Autoreload", this)),
    recent_files(new QMenu("Open recent", this)),
    recent_files_group(new QActionGroup(this)),
    recent_files_clear_action(new QAction("Clear recent files", this)),
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

    autoreload_action->setCheckable(true);
    autoreload_action->setChecked(true);
    QObject::connect(autoreload_action, &QAction::triggered,
            this, &Window::on_autoreload_triggered);

    reload_action->setShortcut(QKeySequence::Refresh);
    QObject::connect(reload_action, &QAction::triggered,
                     this, &Window::on_reload);

    QObject::connect(about_action, &QAction::triggered,
                     this, &Window::on_about);

    QObject::connect(recent_files_clear_action, &QAction::triggered,
                     this, &Window::on_clear_recent);
    QObject::connect(recent_files_group, &QActionGroup::triggered,
                     this, &Window::on_load_recent);

    rebuild_recent_files();

    auto file_menu = menuBar()->addMenu("File");
    file_menu->addAction(open_action);
    file_menu->addMenu(recent_files);
    file_menu->addSeparator();
    file_menu->addAction(reload_action);
    file_menu->addAction(autoreload_action);
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

void Window::on_empty_mesh()
{
    QMessageBox::critical(this, "Error",
                          "<b>Error:</b><br>"
                          "This file is syntactically correct<br>but contains no triangles.");
}

void Window::on_confusing_stl()
{
    QMessageBox::warning(this, "Warning",
                         "<b>Warning:</b><br>"
                         "This <code>.stl</code> file begins with <code>solid </code>but appears to be a binary file.<br>"
                         "<code>fstl</code> loaded it, but other programs may be confused by this file.");
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

    QSettings settings;
    auto recent = settings.value(RECENT_FILE_KEY).toStringList();
    recent.removeAll(filename);
    recent.prepend(QFileInfo(filename).absoluteFilePath());
    while (recent.size() > MAX_RECENT_FILES)
    {
        recent.pop_back();
    }
    settings.setValue(RECENT_FILE_KEY, recent);
    rebuild_recent_files();
}

void Window::on_projection(QAction* proj)
{
    if (proj == perspective_action)
    {
        canvas->view_perspective();
    }
    else
    {
        canvas->view_orthographic();
    }
}

void Window::on_watched_change(const QString& filename)
{
    if (autoreload_action->isChecked())
    {
        load_stl(filename, true);
    }
}

void Window::on_autoreload_triggered(bool b)
{
    if (b)
    {
        on_reload();
    }
}

void Window::on_clear_recent()
{
    QSettings settings;
    settings.setValue(RECENT_FILE_KEY, QStringList());
    rebuild_recent_files();
}

void Window::on_load_recent(QAction* a)
{
    load_stl(a->data().toString());
}

void Window::rebuild_recent_files()
{
    QSettings settings;
    QStringList files = settings.value(RECENT_FILE_KEY).toStringList();

    const auto actions = recent_files_group->actions();
    for (auto a : actions)
    {
        recent_files_group->removeAction(a);
    }
    recent_files->clear();

    for (auto f : files)
    {
        const auto a = new QAction(f, recent_files);
        a->setData(f);
        recent_files_group->addAction(a);
        recent_files->addAction(a);
    }
    if (files.size() == 0)
    {
        auto a = new QAction("No recent files", recent_files);
        recent_files->addAction(a);
        a->setEnabled(false);
    }
    recent_files->addSeparator();
    recent_files->addAction(recent_files_clear_action);
}

void Window::on_reload()
{
    auto fs = watcher->files();
    if (fs.size() == 1)
    {
        load_stl(fs[0], true);
    }
}

bool Window::load_stl(const QString& filename, bool is_reload)
{
    if (!open_action->isEnabled())  return false;

    canvas->set_status("Loading " + filename);

    Loader* loader = new Loader(this, filename, is_reload);
    connect(loader, &Loader::started,
              this, &Window::disable_open);

    connect(loader, &Loader::got_mesh,
            canvas, &Canvas::load_mesh);
    connect(loader, &Loader::error_bad_stl,
              this, &Window::on_bad_stl);
    connect(loader, &Loader::error_empty_mesh,
              this, &Window::on_empty_mesh);
    connect(loader, &Loader::warning_confusing_stl,
              this, &Window::on_confusing_stl);
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
