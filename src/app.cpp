#include "app.h"
#include "window.h"

App::App(int argc, char *argv[]) :
    QApplication(argc, argv), window(new Window())
{
    window->show();
}
