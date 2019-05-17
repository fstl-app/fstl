/**
 * @file main.cpp
 * @brief fstl is a viewer for .stl files build upon Qt5 and OpenGl
 *
 * @details
 * http://www.mattkeeter.com/projects/fstl/
 */

#include <QApplication>

#include "app.h"

int main(int argc, char *argv[])
{
    App a(argc, argv);
    return a.exec();
}
