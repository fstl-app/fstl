#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>

class Canvas;

class Window : public QMainWindow
{
    Q_OBJECT
public:
    explicit Window(QWidget* parent=0);
    bool load_stl(const QString& filename);

protected:
    void dragEnterEvent(QDragEnterEvent* event);
    void dropEvent(QDropEvent* event);

public slots:
    void on_open();
    void on_about();
    void on_ascii_stl();
    void on_bad_stl();

    void enable_open();
    void disable_open();

private:
    QAction* const open_action;
    QAction* const about_action;
    QAction* const quit_action;

    Canvas* canvas;
};

#endif // WINDOW_H
