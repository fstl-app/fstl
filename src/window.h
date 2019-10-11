#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include <QActionGroup>
#include <QFileSystemWatcher>
#include <QCollator>

class Canvas;

class Window : public QMainWindow
{
    Q_OBJECT
public:
    explicit Window(QWidget* parent=0);
    bool load_stl(const QString& filename, bool is_reload=false);
    bool load_prev(void);
    bool load_next(void);

protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

public slots:
    void on_open();
    void on_about();
    void on_bad_stl();
    void on_empty_mesh();
    void on_missing_file();
    void on_confusing_stl();

    void enable_open();
    void disable_open();

    void set_watched(const QString& filename);

private slots:
    void on_projection(QAction* proj);
    void on_drawMode(QAction* mode);
    void on_watched_change(const QString& filename);
    void on_reload();
    void on_autoreload_triggered(bool r);
    void on_clear_recent();
    void on_load_recent(QAction* a);
    void on_loaded(const QString& filename);

private:
    void rebuild_recent_files();
    void sorted_insert(QStringList& list, const QCollator& collator, const QString& value);
    void build_folder_file_list();
    QPair<QString, QString> get_file_neighbors();

    QAction* const open_action;
    QAction* const about_action;
    QAction* const quit_action;
    QAction* const perspective_action;
    QAction* const orthogonal_action;
    QAction* const shaded_action;
    QAction* const wireframe_action;
    QAction* const reload_action;
    QAction* const autoreload_action;

    QMenu* const recent_files;
    QActionGroup* const recent_files_group;
    QAction* const recent_files_clear_action;
    const static int MAX_RECENT_FILES=8;
    const static QString RECENT_FILE_KEY;
    QString current_file;
    QString lookup_folder;
    QStringList lookup_folder_files;

    QFileSystemWatcher* watcher;

    Canvas* canvas;
};

#endif // WINDOW_H
