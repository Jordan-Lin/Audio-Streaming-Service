#include "mainwindow.h"
#include <QApplication>

// Making a comment here ------------------------------------------------TAKE OUT
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
