#include <QMenuBar>

#include "canvas.h"
#include "loader.h"
#include "shaderlightprefs.h"
#include "window.h"

const QString Window::OPEN_EXTERNAL_KEY = "externalCmd";
const QString Window::RECENT_FILE_KEY = "recentFiles";
const QString Window::INVERT_ZOOM_KEY = "invertZoom";
const QString Window::AUTORELOAD_KEY = "autoreload";
const QString Window::DRAW_AXES_KEY = "drawAxes";
const QString Window::PROJECTION_KEY = "projection";
const QString Window::DRAW_MODE_KEY = "drawMode";
const QString Window::WINDOW_GEOM_KEY = "windowGeometry";
const QString Window::RESET_TRANSFORM_ON_LOAD_KEY = "resetTransformOnLoad";

Window::Window(QWidget* parent) :
    QMainWindow(parent),
    open_action(new QAction("Open", this)),
    open_external_action(new QAction("Open with", this)),
    about_action(new QAction("About", this)),
    quit_action(new QAction("Quit", this)),
    perspective_action(new QAction("Perspective", this)),
    common_view_center_action(new QAction("Center the model", this)),
    common_view_iso_action(new QAction("Isometric", this)),
    common_view_top_action(new QAction("Top", this)),
    common_view_bottom_action(new QAction("Bottom", this)),
    common_view_left_action(new QAction("Left", this)),
    common_view_right_action(new QAction("Right", this)),
    common_view_front_action(new QAction("Front", this)),
    common_view_back_action(new QAction("Back", this)),
    orthographic_action(new QAction("Orthographic", this)),
    shaded_action(new QAction("Shaded", this)),
    wireframe_action(new QAction("Wireframe", this)),
    surfaceangle_action(new QAction("Surface Angle", this)),
    meshlight_action(new QAction("Shaded ambient and directive light source", this)),
    drawModePrefs_action(new QAction("Draw Mode Settings")),
    axes_action(new QAction("Draw Axes", this)),
    invert_zoom_action(new QAction("Invert Zoom", this)),
    reload_action(new QAction("Reload", this)),
    autoreload_action(new QAction("Autoreload", this)),
    save_screenshot_action(new QAction("Save Screenshot", this)),
    hide_menuBar_action(new QAction("Hide Menu Bar", this)),
    fullscreen_action(new QAction("Toggle Fullscreen", this)),
    resetTransformOnLoadAction(new QAction("Reset rotation on load", this)),
    recent_files(new QMenu("Open recent", this)),
    recent_files_group(new QActionGroup(this)),
    recent_files_clear_action(new QAction("Clear recent files", this)),
    watcher(new QFileSystemWatcher(this))

{
    setWindowTitle("fstl");
    setWindowIcon(QIcon(":/qt/icons/fstl_64x64.png"));
    setAcceptDrops(true);

    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setVersion(2, 1);
    format.setProfile(QSurfaceFormat::CoreProfile);

    QSurfaceFormat::setDefaultFormat(format);

    canvas = new Canvas(format, this);
    setCentralWidget(canvas);

    meshlightprefs = new ShaderLightPrefs(this, canvas);

    QObject::connect(drawModePrefs_action, &QAction::triggered, this, &Window::on_drawModePrefs);

    QObject::connect(watcher, &QFileSystemWatcher::fileChanged, this, &Window::on_watched_change);

    open_action->setShortcut(QKeySequence::Open);
    QObject::connect(open_action, &QAction::triggered, this, &Window::on_open);
    this->addAction(open_action);

    open_external_action->setShortcut(QKeySequence::Open);
    QObject::connect(open_external_action, &QAction::triggered, this, &Window::on_open_external);
    this->addAction(open_external_action);
    open_external_action->setShortcut(QKeySequence(Qt::ALT + Qt::Key_S));

    QList<QKeySequence> quitShortcuts = {QKeySequence::Quit, QKeySequence::Close};
    quit_action->setShortcuts(quitShortcuts);
    QObject::connect(quit_action, &QAction::triggered, this, &Window::close);
    this->addAction(quit_action);

    autoreload_action->setCheckable(true);
    QObject::connect(autoreload_action, &QAction::triggered, this, &Window::on_autoreload_triggered);

    reload_action->setShortcut(QKeySequence::Refresh);
    reload_action->setEnabled(false);
    QObject::connect(reload_action, &QAction::triggered, this, &Window::on_reload);

    QObject::connect(about_action, &QAction::triggered, this, &Window::on_about);

    QObject::connect(recent_files_clear_action, &QAction::triggered, this, &Window::on_clear_recent);
    QObject::connect(recent_files_group, &QActionGroup::triggered, this, &Window::on_load_recent);

    save_screenshot_action->setCheckable(false);
    QObject::connect(save_screenshot_action, &QAction::triggered, this, &Window::on_save_screenshot);

    rebuild_recent_files();

    const auto file_menu = menuBar()->addMenu("File");
    file_menu->addAction(open_action);
    file_menu->addAction(open_external_action);
    file_menu->addMenu(recent_files);
    file_menu->addSeparator();
    file_menu->addAction(reload_action);
    file_menu->addAction(autoreload_action);
    file_menu->addAction(save_screenshot_action);
    file_menu->addAction(quit_action);

    const auto view_menu = menuBar()->addMenu("View");
    const auto projection_menu = view_menu->addMenu("Projection");
    projection_menu->addAction(perspective_action);
    projection_menu->addAction(orthographic_action);
    const auto projections = new QActionGroup(projection_menu);
    for (auto p : {perspective_action, orthographic_action}) {
        projections->addAction(p);
        p->setCheckable(true);
    }
    projections->setExclusive(true);
    QObject::connect(projections, &QActionGroup::triggered, this, &Window::on_projection);

    const auto draw_menu = view_menu->addMenu("Draw Mode");
    draw_menu->addAction(shaded_action);
    draw_menu->addAction(wireframe_action);
    draw_menu->addAction(surfaceangle_action);
    draw_menu->addAction(meshlight_action);
    const auto drawModes = new QActionGroup(draw_menu);
    for (auto p : {shaded_action, wireframe_action, surfaceangle_action, meshlight_action}) {
        drawModes->addAction(p);
        p->setCheckable(true);
    }
    drawModes->setExclusive(true);
    QObject::connect(drawModes, &QActionGroup::triggered, this, &Window::on_drawMode);
    view_menu->addAction(drawModePrefs_action);
    drawModePrefs_action->setDisabled(true);

    const auto common_menu = view_menu->addMenu("Viewpoint");
    common_menu->addAction(common_view_iso_action);
    common_menu->addAction(common_view_top_action);
    common_menu->addAction(common_view_bottom_action);
    common_menu->addAction(common_view_front_action);
    common_menu->addAction(common_view_back_action);
    common_menu->addAction(common_view_left_action);
    common_menu->addAction(common_view_right_action);
    common_menu->addAction(common_view_center_action);
    const auto common_views = new QActionGroup(common_menu);
    common_views->addAction(common_view_iso_action);
    common_views->addAction(common_view_top_action);
    common_views->addAction(common_view_bottom_action);
    common_views->addAction(common_view_front_action);
    common_views->addAction(common_view_back_action);
    common_views->addAction(common_view_left_action);
    common_views->addAction(common_view_right_action);
    common_views->addAction(common_view_center_action);
    common_view_iso_action->setShortcut(Qt::Key_0);
    common_view_top_action->setShortcut(Qt::Key_1);
    common_view_bottom_action->setShortcut(Qt::Key_2);
    common_view_front_action->setShortcut(Qt::Key_3);
    common_view_back_action->setShortcut(Qt::Key_4);
    common_view_left_action->setShortcut(Qt::Key_5);
    common_view_right_action->setShortcut(Qt::Key_6);
    common_view_center_action->setShortcut(Qt::Key_9);
    QObject::connect(common_views, &QActionGroup::triggered, this, &Window::on_common_view_change);

    view_menu->addAction(axes_action);
    axes_action->setCheckable(true);
    QObject::connect(axes_action, &QAction::triggered, this, &Window::on_drawAxes);

    view_menu->addAction(invert_zoom_action);
    invert_zoom_action->setCheckable(true);
    QObject::connect(invert_zoom_action, &QAction::triggered, this, &Window::on_invertZoom);

    view_menu->addAction(resetTransformOnLoadAction);
    resetTransformOnLoadAction->setCheckable(true);
    QObject::connect(resetTransformOnLoadAction, &QAction::triggered, this, &Window::on_resetTransformOnLoad);

    view_menu->addAction(hide_menuBar_action);
    hide_menuBar_action->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_C);
    hide_menuBar_action->setCheckable(true);
    QObject::connect(hide_menuBar_action, &QAction::toggled, this, &Window::on_hide_menuBar);
    this->addAction(hide_menuBar_action);

    view_menu->addAction(fullscreen_action);
    fullscreen_action->setShortcut(Qt::Key_F11);
    fullscreen_action->setCheckable(true);
    QObject::connect(fullscreen_action, &QAction::toggled, this, &Window::on_fullscreen);
    this->addAction(fullscreen_action);

    auto help_menu = menuBar()->addMenu("Help");
    help_menu->addAction(about_action);

    load_persist_settings();
}

void Window::load_persist_settings()
{
    QSettings settings;
    bool invert_zoom = settings.value(INVERT_ZOOM_KEY, false).toBool();
    canvas->invert_zoom(invert_zoom);
    invert_zoom_action->setChecked(invert_zoom);

    bool resetTransformOnLoad = settings.value(RESET_TRANSFORM_ON_LOAD_KEY, true).toBool();
    canvas->setResetTransformOnLoad(resetTransformOnLoad);
    resetTransformOnLoadAction->setChecked(resetTransformOnLoad);

    autoreload_action->setChecked(settings.value(AUTORELOAD_KEY, true).toBool());

    bool draw_axes = settings.value(DRAW_AXES_KEY, false).toBool();
    canvas->draw_axes(draw_axes);
    axes_action->setChecked(draw_axes);

    QString projection = settings.value(PROJECTION_KEY, "perspective").toString();
    if (projection == "perspective") {
        canvas->view_perspective(Canvas::P_PERSPECTIVE, false);
        perspective_action->setChecked(true);
    } else {
        canvas->view_perspective(Canvas::P_ORTHOGRAPHIC, false);
        orthographic_action->setChecked(true);
    }

    QString path = settings.value(OPEN_EXTERNAL_KEY, "").toString();
    if (!QDir::isAbsolutePath(path) && !path.isEmpty()) {
        path = QStandardPaths::findExecutable(path);
    }
    QString displayName = path.mid(path.lastIndexOf(QDir::separator()) + 1);
    open_external_action->setText("Open with " + displayName);
    open_external_action->setData(path);

    DrawMode draw_mode = (DrawMode)settings.value(DRAW_MODE_KEY, DRAWMODECOUNT).toInt();

    if (draw_mode >= DRAWMODECOUNT) {
        draw_mode = shaded;
    }
    QAction*(dm_acts[]) = {shaded_action, wireframe_action, surfaceangle_action, meshlight_action};
    dm_acts[draw_mode]->setChecked(true);
    on_drawMode(dm_acts[draw_mode]);

    resize(600, 400);
    restoreGeometry(settings.value(WINDOW_GEOM_KEY).toByteArray());
}

void Window::on_drawModePrefs()
{
    // For now only one draw mode has settings
    // when settings for other draw mode will be available
    // we will need to check the current mode
    if (meshlightprefs->isVisible()) {
        meshlightprefs->hide();
    } else {
        meshlightprefs->show();
    }
}

void Window::on_open()
{
    const QString filename = QFileDialog::getOpenFileName(this, "Load .stl file", QString(), "STL files (*.stl *.STL)");
    if (!filename.isNull()) {
        load_stl(filename);
    }
}

void Window::on_open_external() const
{
    if (current_file.isEmpty()) {
        return;
    }

    QString program = open_external_action->data().toString();
    if (program.isEmpty()) {
        program = QFileDialog::getOpenFileName((QWidget*)this, "Select program to open with", QDir::rootPath());
        if (!program.isEmpty()) {
            QSettings settings;
            settings.setValue(OPEN_EXTERNAL_KEY, program);
            QString displayName = program.mid(program.lastIndexOf(QDir::separator()) + 1);
            open_external_action->setText("Open with " + displayName);
            open_external_action->setData(program);
        }
    }

    QProcess::startDetached(program, QStringList(current_file));
}

void Window::on_about()
{
    QMessageBox::about(this, "",
                       "<p align=\"center\"><b>fstl</b><br>" FSTL_VERSION "</p>"
                       "<p>A fast viewer for <code>.stl</code> files.<br>"
                       "<a href=\"https://github.com/fstl-app/fstl\""
                       "   style=\"color: #93a1a1;\">https://github.com/fstl-app/fstl</a></p>"
                       "<p>© 2014-2025 Matthew Keeter<br>"
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
    if (files.size()) {
        watcher->removePaths(watcher->files());
    }
    watcher->addPath(filename);

    QSettings settings;
    auto recent = settings.value(RECENT_FILE_KEY).toStringList();
    const auto f = QFileInfo(filename).absoluteFilePath();
    recent.removeAll(f);
    recent.prepend(f);
    while (recent.size() > MAX_RECENT_FILES) {
        recent.pop_back();
    }
    settings.setValue(RECENT_FILE_KEY, recent);
    rebuild_recent_files();
}

void Window::on_projection(QAction* proj)
{
    if (proj == perspective_action) {
        canvas->view_perspective(Canvas::P_PERSPECTIVE, true);
        QSettings().setValue(PROJECTION_KEY, "perspective");
    } else {
        canvas->view_perspective(Canvas::P_ORTHOGRAPHIC, true);
        QSettings().setValue(PROJECTION_KEY, "orthographic");
    }
}

void Window::on_drawMode(QAction* act)
{
    // On mode change hide prefs first
    meshlightprefs->hide();

    DrawMode mode;
    if (act == shaded_action) {
        drawModePrefs_action->setEnabled(false);
        mode = shaded;
    } else if (act == wireframe_action) {
        drawModePrefs_action->setEnabled(false);
        mode = wireframe;
    } else if (act == surfaceangle_action) {
        drawModePrefs_action->setEnabled(false);
        mode = surfaceangle;
    } else if (act == meshlight_action) {
        drawModePrefs_action->setEnabled(true);
        mode = meshlight;
    }
    canvas->set_drawMode(mode);
    QSettings().setValue(DRAW_MODE_KEY, mode);
}

void Window::on_drawAxes(bool d)
{
    canvas->draw_axes(d);
    QSettings().setValue(DRAW_AXES_KEY, d);
}

void Window::on_invertZoom(bool d)
{
    canvas->invert_zoom(d);
    QSettings().setValue(INVERT_ZOOM_KEY, d);
}

void Window::on_resetTransformOnLoad(bool d)
{
    canvas->setResetTransformOnLoad(d);
    QSettings().setValue(RESET_TRANSFORM_ON_LOAD_KEY, d);
}

void Window::on_watched_change(const QString& filename)
{
    if (autoreload_action->isChecked()) {
        load_stl(filename, true);
    }
}

void Window::on_autoreload_triggered(bool b)
{
    if (b) {
        on_reload();
    }
    QSettings().setValue(AUTORELOAD_KEY, b);
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
        this, tr("Save Screenshot Image"),
        QStandardPaths::standardLocations(QStandardPaths::StandardLocation::PicturesLocation).first(), "Images (*.png *.jpg)");

    auto get_file_extension = [](const std::string& file_name) -> std::string {
        const auto location = std::find(file_name.rbegin(), file_name.rend(), '.');
        if (location == file_name.rend()) {
            return "";
        }

        const auto index = std::distance(file_name.rbegin(), location);
        return file_name.substr(file_name.size() - index);
    };

    const auto extension = get_file_extension(file_name.toStdString());
    if (extension.empty() || (extension != "png" && extension != "jpg")) {
        file_name.append(".png");
    }

    const auto save_ok = image.save(file_name);
    if (!save_ok) {
        QMessageBox::warning(this, tr("Error Saving Image"), tr("Unable to save screen shot image."));
    }
}

void Window::on_hide_menuBar()
{
    menuBar()->setVisible(!hide_menuBar_action->isChecked());
}

void Window::rebuild_recent_files()
{
    QSettings settings;
    QStringList files = settings.value(RECENT_FILE_KEY).toStringList();

    const auto actions = recent_files_group->actions();
    for (auto a : actions) {
        recent_files_group->removeAction(a);
    }
    recent_files->clear();

    for (auto f : files) {
        const auto a = new QAction(f, recent_files);
        a->setData(f);
        recent_files_group->addAction(a);
        recent_files->addAction(a);
    }
    if (files.size() == 0) {
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
    if (fs.size() == 1) {
        load_stl(fs[0], true);
    }
}

void Window::on_common_view_change(QAction* common)
{
    if (common == common_view_center_action)
        canvas->common_view_change(centerview);
    if (common == common_view_iso_action)
        canvas->common_view_change(isoview);
    if (common == common_view_top_action)
        canvas->common_view_change(topview);
    if (common == common_view_bottom_action)
        canvas->common_view_change(bottomview);
    if (common == common_view_left_action)
        canvas->common_view_change(leftview);
    if (common == common_view_right_action)
        canvas->common_view_change(rightview);
    if (common == common_view_front_action)
        canvas->common_view_change(frontview);
    if (common == common_view_back_action)
        canvas->common_view_change(backview);
}

bool Window::load_stl(const QString& filename, bool is_reload)
{
    if (!open_action->isEnabled())
        return false;

    canvas->set_status("Loading " + filename);

    Loader* loader = new Loader(this, filename, is_reload);
    connect(loader, &Loader::started, this, &Window::disable_open);

    connect(loader, &Loader::got_mesh, canvas, &Canvas::load_mesh);
    connect(loader, &Loader::error_bad_stl, this, &Window::on_bad_stl);
    connect(loader, &Loader::error_empty_mesh, this, &Window::on_empty_mesh);
    connect(loader, &Loader::error_missing_file, this, &Window::on_missing_file);

    connect(loader, &Loader::finished, loader, &Loader::deleteLater);
    connect(loader, &Loader::finished, this, &Window::enable_open);
    connect(loader, &Loader::finished, canvas, &Canvas::clear_status);

    if (filename[0] != ':') {
        connect(loader, &Loader::loaded_file, this, &Window::setWindowTitle);
        connect(loader, &Loader::loaded_file, this, &Window::set_watched);
        connect(loader, &Loader::loaded_file, this, &Window::on_loaded);
        reload_action->setEnabled(true);
    }

    loader->start();
    return true;
}

void Window::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasUrls()) {
        auto urls = event->mimeData()->urls();
        if (urls.size() == 1 && urls.front().path().endsWith(".stl"))
            event->acceptProposedAction();
    }
}

void Window::dropEvent(QDropEvent* event)
{
    load_stl(event->mimeData()->urls().front().toLocalFile());
}

void Window::resizeEvent(QResizeEvent* event)
{
    QSettings().setValue(WINDOW_GEOM_KEY, saveGeometry());
    QWidget::resizeEvent(event);
}

void Window::moveEvent(QMoveEvent* event)
{
    QSettings().setValue(WINDOW_GEOM_KEY, saveGeometry());
    QWidget::moveEvent(event);
}

void Window::sorted_insert(QStringList& list, const QCollator& collator, const QString& value)
{
    int start = 0;
    int end = list.size() - 1;
    int index = 0;
    while (start <= end) {
        int mid = (start + end) / 2;
        if (list[mid] == value) {
            return;
        }
        int compare = collator.compare(value, list[mid]);
        if (compare < 0) {
            end = mid - 1;
            index = mid;
        } else {
            start = mid + 1;
            index = start;
        }
    }

    list.insert(index, value);
}

void Window::build_folder_file_list()
{
    QString current_folder_path = QFileInfo(current_file).absoluteDir().absolutePath();
    if (!lookup_folder_files.isEmpty()) {
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
        return QPair<QString, QString>(QString(), QString());
    }

    build_folder_file_list();

    QFileInfo fileInfo(current_file);

    QString current_dir = fileInfo.absoluteDir().absolutePath();
    QString current_name = fileInfo.fileName();

    QString prev = QString();
    QString next = QString();

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
    if (!open_action->isEnabled()) {
        QMainWindow::keyPressEvent(event);
        return;
    }

    if (event->key() == Qt::Key_Left) {
        load_prev();
        return;
    } else if (event->key() == Qt::Key_Right) {
        load_next();
        return;
    } else if (event->key() == Qt::Key_Escape) {
        hide_menuBar_action->setChecked(false);
        return;
    }

    QMainWindow::keyPressEvent(event);
}

void Window::on_fullscreen()
{
    if (!this->isFullScreen()) {
        this->showFullScreen();
    } else {
        this->showNormal();
    }
}
