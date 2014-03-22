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

public slots:
    void on_open();
    void on_about();

    void enable_open();
    void disable_open();

private:
    QAction* const open_action;
    QAction* const about_action;
    QAction* const quit_action;

    Canvas* canvas;
};

#endif // WINDOW_H
