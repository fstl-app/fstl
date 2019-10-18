#include <QMenuBar>

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
    shaded_action(new QAction("Shaded", this)),
    wireframe_action(new QAction("Wireframe", this)),
    reload_action(new QAction("Reload", this)),
    autoreload_action(new QAction("Autoreload", this)),
    save_screenshot_action(new QAction("Save Screenshot", this)),
    recent_files(new QMenu("Open recent", this)),
    recent_files_group(new QActionGroup(this)),
    recent_files_clear_action(new QAction("Clear recent files", this)),
    watcher(new QFileSystemWatcher(this))

{
    setWindowTitle("fstl");
    setAcceptDrops(true);

    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setVersion(2, 1);
    format.setProfile(QSurfaceFormat::CoreProfile);

    QSurfaceFormat::setDefaultFormat(format);
    
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
    autoreload_action->setEnabled(false);
    QObject::connect(autoreload_action, &QAction::triggered,
            this, &Window::on_autoreload_triggered);

    reload_action->setShortcut(QKeySequence::Refresh);
    reload_action->setEnabled(false);
    QObject::connect(reload_action, &QAction::triggered,
                     this, &Window::on_reload);

    QObject::connect(about_action, &QAction::triggered,
                     this, &Window::on_about);

    QObject::connect(recent_files_clear_action, &QAction::triggered,
                     this, &Window::on_clear_recent);
    QObject::connect(recent_files_group, &QActionGroup::triggered,
                     this, &Window::on_load_recent);

    save_screenshot_action->setCheckable(false);
    QObject::connect(save_screenshot_action, &QAction::triggered, 
        this, &Window::on_save_screenshot);
    
    rebuild_recent_files();

    auto file_menu = menuBar()->addMenu("File");
    file_menu->addAction(open_action);
    file_menu->addMenu(recent_files);
    file_menu->addSeparator();
    file_menu->addAction(reload_action);
    file_menu->addAction(autoreload_action);
    file_menu->addAction(save_screenshot_action);
    file_menu->addAction(quit_action);

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

    auto draw_menu = view_menu->addMenu("Draw Mode");
    draw_menu->addAction(shaded_action);
    draw_menu->addAction(wireframe_action);
    auto drawModes = new QActionGroup(draw_menu);
    for (auto p : {shaded_action, wireframe_action})
    {
        drawModes->addAction(p);
        p->setCheckable(true);
    }
    shaded_action->setChecked(true);
    drawModes->setExclusive(true);
    QObject::connect(drawModes, &QActionGroup::triggered,
                     this, &Window::on_drawMode);

    auto help_menu = menuBar()->addMenu("Help");
    help_menu->addAction(about_action);

    resize(600, 400);
}

void Window::on_open()
{
    QString filename = QFileDialog::getOpenFileName(
                this, "Load .stl file", QString(), "*.stl");
    if (!filename.isNull())
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
        "<p>© 2014-2017 Matthew Keeter<br>"
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
    const auto f = QFileInfo(filename).absoluteFilePath();
    recent.removeAll(f);
    recent.prepend(f);
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

void Window::on_drawMode(QAction* mode)
{
    if (mode == shaded_action)
    {
        canvas->draw_shaded();
    }
    else
    {
        canvas->draw_wireframe();
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

void Window::on_loaded(const QString& filename)
{
    current_file = filename;
}

void Window::on_save_screenshot()
{
    const auto image = canvas->grabFramebuffer();
    auto file_name = QFileDialog::getSaveFileName(
        this, 
        tr("Save Screenshot Image"),
        QStandardPaths::standardLocations(QStandardPaths::StandardLocation::PicturesLocation).first(),
        "Images (*.png *.jpg)");

    auto get_file_extension = [](const std::string& file_name) -> std::string
    {
        const auto location = std::find(file_name.rbegin(), file_name.rend(), '.');
        if (location == file_name.rend())
        {
            return "";
        }

        const auto index = std::distance(file_name.rbegin(), location);
        return file_name.substr(file_name.size() - index);
    };

    const auto extension = get_file_extension(file_name.toStdString());
    if(extension.empty() || (extension != "png" && extension != "jpg"))
    {
        file_name.append(".png");
    }
    
    const auto save_ok = image.save(file_name);
    if(!save_ok)
    {
        QMessageBox::warning(this, tr("Error Saving Image"), tr("Unable to save screen shot image."));
    }
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
        connect(loader, &Loader::loaded_file,
                  this, &Window::on_loaded);
        autoreload_action->setEnabled(true);
        reload_action->setEnabled(true);
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

void Window::sorted_insert(QStringList& list, const QCollator& collator, const QString& value)
{
    int start = 0;
    int end = list.size() - 1;
    int index = 0;
    while (start <= end){
        int mid = (start+end)/2;
        if (list[mid] == value) {
            return;
        }
        int compare = collator.compare(value, list[mid]);
        if (compare < 0) {
            end = mid-1;
            index = mid;
        } else {
            start = mid+1;
            index = start;
        }
    }

    list.insert(index, value);
}

void Window::build_folder_file_list()
{
    QString current_folder_path = QFileInfo(current_file).absoluteDir().absolutePath();
    if (!lookup_folder_files.isEmpty())
    {
        if (current_folder_path == lookup_folder) {
            return;
        }

        lookup_folder_files.clear();
    }
    lookup_folder = current_folder_path;

    QCollator collator;
    collator.setNumericMode(true);

    QDirIterator dirIterator(lookup_folder, QStringList() << "*.stl", QDir::Files | QDir::Readable | QDir::Hidden);
    while (dirIterator.hasNext()) {
        dirIterator.next();

        QString name = dirIterator.fileName();
        sorted_insert(lookup_folder_files, collator, name);
    }
}

QPair<QString, QString> Window::get_file_neighbors()
{
    if (current_file.isEmpty()) {
        return QPair<QString, QString>(QString::null, QString::null);
    }

    build_folder_file_list();

    QFileInfo fileInfo(current_file);

    QString current_dir = fileInfo.absoluteDir().absolutePath();
    QString current_name = fileInfo.fileName();

    QString prev = QString::null;
    QString next = QString::null;

    QListIterator<QString> fileIterator(lookup_folder_files);
    while (fileIterator.hasNext()) {
        QString name = fileIterator.next();

        if (name == current_name) {
            if (fileIterator.hasNext()) {
                next = current_dir + QDir::separator() + fileIterator.next();
            }
            break;
        }

        prev = name;
    }

    if (!prev.isEmpty()) {
        prev.prepend(QDir::separator());
        prev.prepend(current_dir);
    }

    return QPair<QString, QString>(prev, next);
}

bool Window::load_prev(void)
{
    QPair<QString, QString> neighbors = get_file_neighbors();
    if (neighbors.first.isEmpty()) {
        return false;
    }

    return load_stl(neighbors.first);
}

bool Window::load_next(void)
{
    QPair<QString, QString> neighbors = get_file_neighbors();
    if (neighbors.second.isEmpty()) {
        return false;
    }

    return load_stl(neighbors.second);
}

void Window::keyPressEvent(QKeyEvent* event)
{
    if (!open_action->isEnabled())
    {
        QMainWindow::keyPressEvent(event);
        return;
    }

    if (event->key() == Qt::Key_Left)
    {
        load_prev();
        return;
    }
    else if (event->key() == Qt::Key_Right)
    {
        load_next();
        return;
    }

    QMainWindow::keyPressEvent(event);
}
