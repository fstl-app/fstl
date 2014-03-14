#include <QDebug>
#include <QFileOpenEvent>

#include "app.h"
#include "window.h"

App::App(int argc, char *argv[]) :
    QApplication(argc, argv), window(new Window())
{
    window->show();
}

bool App::event(QEvent* e)
{
    if (e->type() == QEvent::FileOpen)
    {
        qDebug() << static_cast<QFileOpenEvent*>(e)->file();
    }
    else
    {
        return QApplication::event(e);
    }
}
