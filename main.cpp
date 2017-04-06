#include "mainwindow.h"
#include <QApplication>
#include "utilities.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    startup();
    MainWindow::get();
    MainWindow::get()->show();

    return a.exec();
}
