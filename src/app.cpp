#include <QDebug>
#include <QFileOpenEvent>
#include <QDir>

#include "app.h"
#include "window.h"

App::App(int& argc, char *argv[]) :
    QApplication(argc, argv), window(new Window())
{
    if (argc > 1) {
		QString filename = argv[1];
		if (filename.startsWith("~")) {
			filename.replace(0, 1, QDir::homePath());
		}
		window->load_stl(filename);
	}
    else
	{
		window->load_stl(":gl/sphere.stl");
	}
    window->show();
}

App::~App()
{
    delete window;
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
