#include <QApplication>

#include "app.h"

#include <iostream>
#include <cstring>

int main(int argc, char* argv[])
{
    // Force C locale to force decimal point
    QLocale::setDefault(QLocale::c());

    QCoreApplication::setOrganizationName("fstl-app");
    QCoreApplication::setOrganizationDomain("https://github.com/fstl-app/fstl");
    QCoreApplication::setApplicationName("fstl");
    QCoreApplication::setApplicationVersion(FSTL_VERSION);

    // If user asked for the version, print and exit immediately.
    // Use raw argv comparison to avoid any QString conversion edge-cases.
    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--version") == 0 || std::strcmp(argv[i], "-V") == 0) {
            std::cout << QCoreApplication::applicationName().toStdString() << " "
                      << QCoreApplication::applicationVersion().toStdString() << std::endl;
            return 0;
        }
    }

    App a(argc, argv);

    return a.exec();
}
