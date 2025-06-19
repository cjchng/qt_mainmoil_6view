#include "mainwindow.h"
#include <QApplication>
#include <QLibrary>


int main(int argc, char *argv[])
{
#ifdef _WIN32
    QLibrary Library("moildev.dll");
#else
    QLibrary Library("libmoildev.so");
#endif
    Library.load();
    QApplication a(argc, argv);
    MainWindow w;
    w.showMaximized();

    return a.exec();
}
