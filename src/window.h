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

private:
    Canvas* canvas;
};

#endif // WINDOW_H
