#include <QMenuBar>

#include "window.h"
#include "canvas.h"
#include "loader.h"
#include "shaderlightprefs.h"

#include <sys/stat.h>
#include <thread>

const QString Window::RECENT_FILE_KEY = "recentFiles";
const QString Window::INVERT_ZOOM_KEY = "invertZoom";
const QString Window::AUTORELOAD_KEY = "autoreload";
const QString Window::DRAW_AXES_KEY = "drawAxes";
const QString Window::PROJECTION_KEY = "projection";
const QString Window::DRAW_MODE_KEY = "drawMode";
const QString Window::WINDOW_GEOM_KEY = "windowGeometry";
const QString Window::RESET_TRANSFORM_ON_LOAD_KEY = "resetTransformOnLoad";
const QString Window::HIDE_MENU_BAR = "hideMenuBar";

const QKeySequence Window::shortcutOpen = Qt::Key_O;
const QKeySequence Window::shortcutReload = Qt::Key_R;
const QKeySequence Window::shortcutScreenshot = Qt::Key_S;
const QKeySequence Window::shortcutQuit = Qt::Key_Q;
const QKeySequence Window::shortcutDrawModeSettings = Qt::Key_P;
const QKeySequence Window::shortcutDrawAxes = Qt::Key_A;
const QKeySequence Window::shortcutHideMenuBar = Qt::Key_M;
const QKeySequence Window::shortcutFullscreen = Qt::Key_F;

Window::Window(QWidget *parent) :
    QMainWindow(parent),
    open_action(new QAction("Open", this)),
    about_action(new QAction("About", this)),
    quit_action(new QAction("Quit", this)),
    perspective_action(new QAction("Perspective", this)),
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
    fullscreen_action(new QAction("Toggle Fullscreen",this)),
    resetTransformOnLoadAction(new QAction("Reset rotation on load",this)),
    setGLSizeAction(new QAction("Set Viewport Size",this)),
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
    canvas->update();

    meshlightprefs = new ShaderLightPrefs(this, canvas);

    QObject::connect(drawModePrefs_action, &QAction::triggered,this,&Window::on_drawModePrefs);

    QObject::connect(watcher, &QFileSystemWatcher::fileChanged,
                     this, &Window::on_watched_change);

    //open_action->setShortcut(QKeySequence::Open);
    open_action->setShortcut(shortcutOpen);
    open_action->setIcon(QIcon(":/qt/icons/document-open.png"));
    QObject::connect(open_action, &QAction::triggered,
                     this, &Window::on_open);
    this->addAction(open_action);

    //quit_action->setShortcut(QKeySequence::Quit);
    quit_action->setShortcut(shortcutQuit);
    quit_action->setIcon(QIcon(":/qt/icons/exit.png"));
    QObject::connect(quit_action, &QAction::triggered,
                     this, &Window::close);
    this->addAction(quit_action);

    autoreload_action->setCheckable(true);
    autoreload_action->setIcon(QIcon(":/qt/icons/auto_refresh.png"));
    QObject::connect(autoreload_action, &QAction::triggered,
            this, &Window::on_autoreload_triggered);

    //reload_action->setShortcut(QKeySequence::Refresh);
    reload_action->setShortcut(shortcutReload);
    reload_action->setIcon(QIcon(":/qt/icons/view-refresh.png"));
    this->addAction(reload_action);
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
    save_screenshot_action->setShortcut(shortcutScreenshot);
    save_screenshot_action->setIcon(QIcon(":/qt/icons/screenshot.png"));
    this->addAction(save_screenshot_action);
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
    projection_menu = view_menu->addMenu("Projection");
    projection_menu->addAction(perspective_action);
    perspective_action->setIcon(QIcon(":/qt/icons/perspective.png"));
    projection_menu->addAction(orthographic_action);
    orthographic_action->setIcon(QIcon(":/qt/icons/orthographic.png"));
    auto projections = new QActionGroup(projection_menu);
    for (auto p : {perspective_action, orthographic_action})
    {
        projections->addAction(p);
        p->setCheckable(true);
    }
    projections->setExclusive(true);
    QObject::connect(projections, &QActionGroup::triggered,
                     this, &Window::on_projection);

    draw_menu = view_menu->addMenu("Draw Mode");
    draw_menu->addAction(shaded_action);
    draw_menu->addAction(wireframe_action);
    draw_menu->addAction(surfaceangle_action);
    draw_menu->addAction(meshlight_action);
    shaded_action->setIcon(QIcon(":/qt/icons/sphere_shader1.png"));
    wireframe_action->setIcon(QIcon(":/qt/icons/sphere_shader2.png"));
    surfaceangle_action->setIcon(QIcon(":/qt/icons/sphere_shader3.png"));
    meshlight_action->setIcon(QIcon(":/qt/icons/sphere_shader4.png"));
    auto drawModes = new QActionGroup(draw_menu);
    for (auto p : {shaded_action, wireframe_action, surfaceangle_action, meshlight_action})
    {
        drawModes->addAction(p);
        p->setCheckable(true);
    }
    drawModes->setExclusive(true);
    QObject::connect(drawModes, &QActionGroup::triggered,
                     this, &Window::on_drawMode);
    view_menu->addAction(drawModePrefs_action);
    drawModePrefs_action->setShortcut(shortcutDrawModeSettings);
    drawModePrefs_action->setIcon(QIcon(":/qt/icons/preferences-system.png"));
    this->addAction(drawModePrefs_action);
    drawModePrefs_action->setDisabled(true);
    view_menu->addAction(axes_action);
    axes_action->setCheckable(true);
    axes_action->setShortcut(shortcutDrawAxes);
    axes_action->setIcon(QIcon(":/qt/icons/axes.png"));
    this->addAction(axes_action);
    QObject::connect(axes_action, &QAction::toggled,
            this, &Window::on_drawAxes);

    view_menu->addAction(invert_zoom_action);
    invert_zoom_action->setCheckable(true);
    invert_zoom_action->setIcon(QIcon(":/qt/icons/invert_zoom.png"));
    QObject::connect(invert_zoom_action, &QAction::triggered,
            this, &Window::on_invertZoom);       

    view_menu->addAction(resetTransformOnLoadAction);
    resetTransformOnLoadAction->setCheckable(true);
    resetTransformOnLoadAction->setIcon(QIcon(":/qt/icons/reset_rotation_on_load.png"));
    QObject::connect(resetTransformOnLoadAction, &QAction::triggered,
            this, &Window::on_resetTransformOnLoad);

    view_menu->addAction(hide_menuBar_action);
    hide_menuBar_action->setShortcut(shortcutHideMenuBar);
    hide_menuBar_action->setCheckable(true);
    QObject::connect(hide_menuBar_action, &QAction::toggled,
            this, &Window::on_hide_menuBar);
    // To have the shortcut work without the menu
    this->addAction(hide_menuBar_action);

    view_menu->addAction(fullscreen_action);
    fullscreen_action->setShortcut(shortcutFullscreen);
    fullscreen_action->setIcon(QIcon(":/qt/icons/view-fullscreen.png"));
    fullscreen_action->setCheckable(true);
    QObject::connect(fullscreen_action, &QAction::toggled,
            this, &Window::on_fullscreen);
    this->addAction(fullscreen_action);

    QMenu *resolutionMenu = view_menu->addMenu("Set Viewport Size");
    resolutionMenu->setIcon(QIcon(":/qt/icons/resolution_1_32.png"));
    resolutionMenu->menuAction()->setIconVisibleInMenu(true);
    QActionGroup* groupResolution = new QActionGroup(resolutionMenu);

    QAction *quatreTiers = new QAction("-- 4:3",this);
    quatreTiers->setDisabled(true);
    resolutionMenu->addAction(quatreTiers);
    QAction *setResolution0Action = new QAction("640 x 480  (VGA)",this);
    resolutionMenu->addAction(setResolution0Action);
    groupResolution->addAction(setResolution0Action);

    QAction *setResolution1Action = new QAction("768 x 576  (PAL)",this);
    resolutionMenu->addAction(setResolution1Action);
    groupResolution->addAction(setResolution1Action);

    QAction *setResolution2Action = new QAction("800 x 600  (SVGA)",this);
    resolutionMenu->addAction(setResolution2Action);
    groupResolution->addAction(setResolution2Action);

    QAction *setResolution3Action = new QAction("1024 x 768 (XGA)",this);
    resolutionMenu->addAction(setResolution3Action);
    groupResolution->addAction(setResolution3Action);

    QAction *seizeNeuf = new QAction("-- 16:9",this);
    seizeNeuf->setDisabled(true);
    resolutionMenu->addAction(seizeNeuf);

    QAction *setResolution4Action = new QAction("800 x 480  (WVGA)",this);
    resolutionMenu->addAction(setResolution4Action);
    groupResolution->addAction(setResolution4Action);

    QAction *setResolution5Action = new QAction("1024 x 576 (16:9 PAL)",this);
    resolutionMenu->addAction(setResolution5Action);
    groupResolution->addAction(setResolution5Action);

    QAction *setResolution6Action = new QAction("1280 x 720 (HD720)",this);
    resolutionMenu->addAction(setResolution6Action);
    groupResolution->addAction(setResolution6Action);

    resolutionMenu->addSeparator();

//    QAction *setCustomResolutionAction = new QAction("Custom Resolution",this);
//    //connect(setCustomResolutionAction,SIGNAL(triggered(bool)),this,SLOT(setCustomResolution()));
//    resolutionMenu->addAction(setCustomResolutionAction);

    connect(groupResolution,SIGNAL(triggered(QAction*)),this,SLOT(setViewportSize(QAction*)));

    auto help_menu = menuBar()->addMenu("Help");
    help_menu->addAction(about_action);

    // Toolbar
    // First group
    windowToolBar = new QToolBar;
    windowToolBar->addAction(quit_action);
    windowToolBar->addAction(open_action);
    windowToolBar->addAction(reload_action);
    windowToolBar->addAction(autoreload_action);

    // preferences button here
    windowToolBar->addSeparator();

    // Second group
    projectionButton = new QToolButton;
    projectionButton->setPopupMode(QToolButton::InstantPopup);
    projectionButton->setMenu(projection_menu);
    projectionButton->setFocusPolicy(Qt::NoFocus); // we do not want the button to have keyboard focus
    windowToolBar->addWidget(projectionButton);

    shaderButton = new QToolButton;
    shaderButton->setPopupMode(QToolButton::InstantPopup);
    shaderButton->setMenu(draw_menu);
    shaderButton->setFocusPolicy(Qt::NoFocus); // we do not want the button to have keyboard focus
    windowToolBar->addWidget(shaderButton);
    windowToolBar->addAction(drawModePrefs_action);

    windowToolBar->addAction(axes_action);
    windowToolBar->addAction(invert_zoom_action);
    windowToolBar->addAction(resetTransformOnLoadAction);

    windowToolBar->addSeparator();
    // Third group

    QToolButton* viewportSizeButton = new QToolButton;
    viewportSizeButton->setPopupMode(QToolButton::InstantPopup);
    viewportSizeButton->setMenu(resolutionMenu);
    viewportSizeButton->setIcon(resolutionMenu->icon());
    viewportSizeButton->setToolTip(resolutionMenu->title());
    viewportSizeButton->setFocusPolicy(Qt::NoFocus); // we do not want the button to have keyboard focus
    windowToolBar->addWidget(viewportSizeButton);

    windowToolBar->addAction(save_screenshot_action);
    windowToolBar->addAction(fullscreen_action);




    // reset view here
    // select views here
    // slect shader here
    // select gl size here






    this->addToolBar(windowToolBar);
    load_persist_settings();
}

void Window::load_persist_settings(){
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
    QAction* currentProjection;
    if(projection == "perspective"){
        canvas->view_perspective(Canvas::P_PERSPECTIVE, false);
        currentProjection = perspective_action;
    }else{
        canvas->view_perspective(Canvas::P_ORTHOGRAPHIC, false);
        currentProjection = orthographic_action;
    }
    currentProjection->setChecked(true);
    on_projection(currentProjection);

    DrawMode draw_mode = (DrawMode)settings.value(DRAW_MODE_KEY, DRAWMODECOUNT).toInt();
    
    if(draw_mode >= DRAWMODECOUNT)
    {
        draw_mode = shaded;
    }
    dm_acts = {shaded_action, wireframe_action, surfaceangle_action, meshlight_action};
    dm_acts[draw_mode]->setChecked(true);
    on_drawMode(dm_acts[draw_mode]);

    // menu bar
    bool hideMenu = settings.value(HIDE_MENU_BAR, false).toBool();
    hide_menuBar_action->blockSignals(true);
    hide_menuBar_action->setChecked(hideMenu);
    on_hide_menuBar();
    hide_menuBar_action->blockSignals(false);

    resize(600, 400);
    restoreGeometry(settings.value(WINDOW_GEOM_KEY).toByteArray());
    if (this->isFullScreen()) {
        fullscreen_action->blockSignals(true);
        fullscreen_action->setChecked(true);
        fullscreen_action->blockSignals(false);
    }
 }

void Window::on_drawModePrefs() {
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
    const QString filename = QFileDialog::getOpenFileName(
                this, "Load .stl file", QString(), "STL files (*.stl *.STL)");
    if (!filename.isNull())
    {
        load_stl(filename);
    }
}

void Window::on_about()
{
    QMessageBox::about(this, "",
        "<p align=\"center\"><b>fstl</b><br>" FSTL_VERSION "</p>"
        "<p>A fast viewer for <code>.stl</code> files.<br>"
        "<a href=\"https://github.com/fstl-app/fstl\""
        "   style=\"color: #93a1a1;\">https://github.com/fstl-app/fstl</a></p>"
        "<p>© 2014-2022 Matthew Keeter<br>"
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
        canvas->view_perspective(Canvas::P_PERSPECTIVE, true);
        QSettings().setValue(PROJECTION_KEY, "perspective");
    }
    else
    {
        canvas->view_perspective(Canvas::P_ORTHOGRAPHIC, true);
        QSettings().setValue(PROJECTION_KEY, "orthographic");
    }
    projection_menu->setIcon(proj->icon());
    projectionButton->setIcon(proj->icon());
    projectionButton->setToolTip(QString("%1 : %2").arg(projection_menu->title()).arg(proj->toolTip()));
}

void Window::on_drawMode(QAction* act)
{
    // On mode change hide prefs first
    meshlightprefs->hide();

    DrawMode mode;
    if (act == shaded_action)
    {
        drawModePrefs_action->setEnabled(false);
        mode = shaded;
    }
    else if (act == wireframe_action)
    {
        drawModePrefs_action->setEnabled(false);
        mode = wireframe;
    }
    else if (act == surfaceangle_action)
    {
        drawModePrefs_action->setEnabled(false);
        mode = surfaceangle;
    }
    else if (act == meshlight_action)
    {
        drawModePrefs_action->setEnabled(true);
        mode = meshlight;
    }
    canvas->set_drawMode(mode);
    QSettings().setValue(DRAW_MODE_KEY, mode);
    draw_menu->setIcon(act->icon());
    shaderButton->setIcon(act->icon());
    shaderButton->setToolTip(QString("%1 : %2").arg(draw_menu->title()).arg(act->toolTip()));
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

void Window::on_resetTransformOnLoad(bool d) {
    canvas->setResetTransformOnLoad(d);
    QSettings().setValue(RESET_TRANSFORM_ON_LOAD_KEY, d);
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

void Window::on_hide_menuBar()
{
    menuBar()->setVisible(!hide_menuBar_action->isChecked());
    windowToolBar->setVisible(!hide_menuBar_action->isChecked());
    QSettings settings;
    settings.setValue(HIDE_MENU_BAR,hide_menuBar_action->isChecked());
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

    struct stat st;
    int file_size, file_size_old;
    QByteArray ba = filename.toLatin1();
    stat(ba.data(), &st);
    file_size = st.st_size;
    do { // wait until file size does not change anymore within 10 ms
        file_size_old = file_size;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        stat(ba.data(), &st);
        file_size = st.st_size;
    } while (file_size != file_size_old);

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

void Window::resizeEvent(QResizeEvent *event)
{
    QSettings().setValue(WINDOW_GEOM_KEY, saveGeometry());
    QWidget::resizeEvent(event);
}

void Window::moveEvent(QMoveEvent *event)
{
    QSettings().setValue(WINDOW_GEOM_KEY, saveGeometry());
    QWidget::moveEvent(event);
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
    } else if (event->key() == Qt::Key_Up) {
        cycleShader(true);
        return;
    } else if (event->key() == Qt::Key_Down) {
        cycleShader(false);
        return;
    } else if (event->key() == Qt::Key_Escape && !menuBar()->isVisible()) { // this is if user did not noticed the hide menu key
        hide_menuBar_action->toggle();
        return;
    } else if (event->key() == Qt::Key_W) {
        if (dm_acts.at(getCurrentShader()) == meshlight_action) {
            meshlightprefs->toggleUseWire();
        }
    }

    QMainWindow::keyPressEvent(event);
}

void Window::on_fullscreen() {
    if (!this->isFullScreen()) {
        this->showFullScreen();
    } else {
        this->showNormal();
    }
}

int Window::getCurrentShader() {
    int shadeNumber = dm_acts.size();
    int current = 0;
    for (int i=0; i<shadeNumber;i++) {
        if (dm_acts.at(i)->isChecked()) {
            current = i;
            break;
        }
    }
    return current;
}

void Window::cycleShader(bool up) {
    int current = getCurrentShader();
    int updown = up ? 1 : -1;
    int nextS = (current + updown) % dm_acts.size();
    nextS = nextS < 0 ? dm_acts.size() - 1 : nextS;
    dm_acts.at(nextS)->setChecked(true);
    on_drawMode(dm_acts.at(nextS));
}


// Resize the widget giving the canvas dimension
// Useful for screenshot of given size.
void Window::setCanvasSize(int w, int h) {
    if (this->isFullScreen()) {
        fullscreen_action->toggle();
    }
    int dw = this->size().width() - canvas->size().width();
    int dh = this->size().height() - canvas->size().height();
    this->resize(w + dw, h + dh);
}

void Window::setViewportSize(QAction* act) {
    QString t = act->text();
    QRegExp rx = QRegExp("^\\s*(\\d+).+(\\d+).*");
    rx.indexIn(t);
    QStringList desc = rx.capturedTexts();
    int w = desc.at(1).toInt();
    int h = desc.at(2).toInt();
    setCanvasSize(w, h);
}
