#include "mainwindow.h"
#include <QApplication>
#include <QLibrary>


int main(int argc, char *argv[])
{
    QLibrary Library("libmoildev.so");
    Library.load();
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
