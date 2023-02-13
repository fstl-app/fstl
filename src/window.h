#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include <QActionGroup>
#include <QFileSystemWatcher>
#include <QCollator>
#include <QToolButton>

class Canvas;
class ShaderLightPrefs;

class Window : public QMainWindow
{
    Q_OBJECT
public:
    explicit Window(QWidget* parent=0);
    bool load_stl(const QString& filename, bool is_reload=false);
    bool load_prev(void);
    bool load_next(void);
    int getCurrentShader();
    void cycleShader(bool);
    void setCanvasSize(int w, int h);

protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    void resizeEvent(QResizeEvent *event) override;
    void moveEvent(QMoveEvent *event) override;
    void keyPressEvent(QKeyEvent* event) override;

public slots:
    void on_open();
    void on_about();
    void on_bad_stl();
    void on_empty_mesh();
    void on_missing_file();

    void enable_open();
    void disable_open();

    void set_watched(const QString& filename);

private slots:
    void on_projection(QAction* proj);
    void on_drawMode(QAction* mode);
    void on_drawAxes(bool d);
    void on_invertZoom(bool d);
    void on_resetTransformOnLoad(bool d);
    void on_watched_change(const QString& filename);
    void on_reload();
    void on_autoreload_triggered(bool r);
    void on_clear_recent();
    void on_load_recent(QAction* a);
    void on_loaded(const QString& filename);
    void on_save_screenshot();
    void on_fullscreen();
    void on_hide_menuBar();
    void on_drawModePrefs();
    void setViewportSize(QAction* act);

private:
    void rebuild_recent_files();
    void load_persist_settings();
    void sorted_insert(QStringList& list, const QCollator& collator, const QString& value);
    void build_folder_file_list();
    QPair<QString, QString> get_file_neighbors();

    QAction* const open_action;
    QAction* const about_action;
    QAction* const quit_action;
    QAction* const perspective_action;
    QAction* const orthographic_action;
    QAction* const shaded_action;
    QAction* const wireframe_action;
    QAction* const surfaceangle_action;
    QAction* const meshlight_action;
    QAction* const drawModePrefs_action;
    QAction* const axes_action;
    QAction* const invert_zoom_action;
    QAction* const reload_action;
    QAction* const autoreload_action;
    QAction* const save_screenshot_action;
    QAction* const hide_menuBar_action;
    QAction* const fullscreen_action;
    QAction* const resetTransformOnLoadAction;
    QAction* const setGLSizeAction;

    QMenu* const recent_files;
    QMenu* draw_menu;
    QToolButton* shaderButton;
    QToolBar* windowToolBar;
    QMenu* projection_menu;
    QToolButton* projectionButton;

    QActionGroup* const recent_files_group;
    QAction* const recent_files_clear_action;
    const static int MAX_RECENT_FILES=8;
    const static QString RECENT_FILE_KEY;
    const static QString INVERT_ZOOM_KEY;
    const static QString AUTORELOAD_KEY;
    const static QString DRAW_AXES_KEY;
    const static QString PROJECTION_KEY;
    const static QString DRAW_MODE_KEY;
    const static QString WINDOW_GEOM_KEY;
    const static QString RESET_TRANSFORM_ON_LOAD_KEY;
    const static QString HIDE_MENU_BAR;


    const static QKeySequence shortcutOpen;
    const static QKeySequence shortcutReload;
    const static QKeySequence shortcutScreenshot;
    const static QKeySequence shortcutQuit;
    const static QKeySequence shortcutDrawModeSettings;
    const static QKeySequence shortcutDrawAxes;
    const static QKeySequence shortcutHideMenuBar;
    const static QKeySequence shortcutFullscreen;

    QString current_file;
    QString lookup_folder;
    QStringList lookup_folder_files;

    QFileSystemWatcher* watcher;

    Canvas* canvas;

    ShaderLightPrefs* meshlightprefs;
    QList<QAction*> dm_acts;
};

#endif // WINDOW_H
