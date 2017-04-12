#include "mainwindow.h"
#include "updatehandler.h"
#include "debugwindow.h"
#include "utilities.h"
#include <QApplication>
#include <QObject>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    //DebugWindow::get()->show();
    w.show();
    startup();

    QObject::connect(UpdateHandler::get(),SIGNAL(updateSongVector()), &w, SLOT(updatedSList()));
    QObject::connect(UpdateHandler::get(), SIGNAL(updateQueueVector()), &w, SLOT(updatedQList()));
    QObject::connect(UpdateHandler::get(), SIGNAL(updateUserVector()), &w, SLOT(updatedUList()));
    QObject::connect(UpdateHandler::get(), SIGNAL(uploadSignal()), &w, SLOT(uploadS()));

    return a.exec();
}
