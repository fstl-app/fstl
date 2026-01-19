#include <iostream>
#include <QApplication>
#include <QCommandLineParser>

#include "app.h"

int main(int argc, char* argv[])
{
    // Force C locale to force decimal point
    QLocale::setDefault(QLocale::c());

    QCoreApplication::setOrganizationName("fstl-app");
    QCoreApplication::setOrganizationDomain("https://github.com/fstl-app/fstl");
    QCoreApplication::setApplicationName("fstl");
    QCoreApplication::setApplicationVersion(FSTL_VERSION);

    App a(argc, argv);

    // Set up QCommandLineParser to handle --help, --version, and file argument
    QCommandLineParser parser;
    parser.setApplicationDescription("Fast .stl file viewer");
    parser.addHelpOption();
    
    // Add custom version option with both -V (uppercase, POSIX) and --version
    QCommandLineOption versionOption(QStringList{"V", "version"}, "Displays version information.");
    parser.addOption(versionOption);
    
    parser.addPositionalArgument("file", "STL file to open (optional)", "[file]");
    parser.process(a);
    
    // Handle version option manually since we use custom -V
    if (parser.isSet(versionOption)) {
        std::cout << QCoreApplication::applicationName().toStdString() << " "
                  << QCoreApplication::applicationVersion().toStdString() << "\n";
        return 0;
    }

    return a.exec();
}
