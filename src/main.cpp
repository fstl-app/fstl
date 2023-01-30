#include <QApplication>

#include "app.h"

int main(int argc, char *argv[])
{
    // Force C locale to force decimal point
    QLocale::setDefault(QLocale::c());

    QCoreApplication::setOrganizationName("fstl-app");
    QCoreApplication::setOrganizationDomain("https://github.com/fstl-app/fstl");
    QCoreApplication::setApplicationName("fstl");
    QCoreApplication::setApplicationVersion(FSTL_VERSION);
    App a(argc, argv);

    return a.exec();
}
