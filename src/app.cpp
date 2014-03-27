#include <QDebug>
#include <QFileOpenEvent>

#include "app.h"
#include "window.h"

App::App(int argc, char *argv[]) :
    QApplication(argc, argv), window(new Window())
{
    window->show();
    if (argc > 1)
        window->load_stl(argv[1]);
    else
        window->load_stl(":gl/sphere.stl");
}

bool App::event(QEvent* e)
{
    if (e->type() == QEvent::FileOpen)
    {
        window->load_stl(static_cast<QFileOpenEvent*>(e)->file());
        return true;
    }
    else
    {
        return QApplication::event(e);
    }
}
