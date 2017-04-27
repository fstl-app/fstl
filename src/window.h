#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include <QFileSystemWatcher>

class Canvas;

class Window : public QMainWindow
{
    Q_OBJECT
public:
    explicit Window(QWidget* parent=0);
    bool load_stl(const QString& filename, bool is_reload=false);

protected:
    void dragEnterEvent(QDragEnterEvent* event);
    void dropEvent(QDropEvent* event);

public slots:
    void on_open();
    void on_about();
    void on_bad_stl();
    void on_missing_file();
    void on_confusing_stl();

    void enable_open();
    void disable_open();

    void set_watched(const QString& filename);

private slots:
    void on_projection(QAction* proj);
    void on_watched_change(const QString& filename);

private:
    QAction* const open_action;
    QAction* const about_action;
    QAction* const quit_action;
    QAction* const perspective_action;
    QAction* const orthogonal_action;

    QFileSystemWatcher* watcher;

    Canvas* canvas;
};

#endif // WINDOW_H
