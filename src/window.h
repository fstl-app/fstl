#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>

class Canvas;

class Window : public QMainWindow
{
    Q_OBJECT
public:
    explicit Window(QWidget* parent=0);
    void load_stl(const QString& filename);

public slots:
    void on_open();
    void on_about();
    void disable_open_action();
    void enable_open_action();

private:
    QAction* open_action;
    QAction* about_action;
    QAction* quit_action;

    Canvas* canvas;
};

#endif // WINDOW_H
