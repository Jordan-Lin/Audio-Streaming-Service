#include "mainwindow.h"
#include "updatehandler.h"
#include "debugwindow.h"
#include "utilities.h"
#include <QApplication>
#include <QObject>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DebugWindow::get()->show();
    MainWindow w;
    w.show();
    startup();

    QObject::connect(UpdateHandler::get(),SIGNAL(updateSongVector(std::vector<std::string>)), &w, SLOT(updatedSList(std::vector<std::string>)));
    QObject::connect(UpdateHandler::get(), SIGNAL(updateQueueVector(std::vector<std::string>)), &w, SLOT(updatedQList(std::vector<std::string>)));
    QObject::connect(UpdateHandler::get(), SIGNAL(updateUserVector(std::vector<std::string>)), &w, SLOT(updatedUList(std::vector<std::string>)));

    return a.exec();
}
