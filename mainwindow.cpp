#pragma comment(lib, "ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "songstreamer.h"
#include "utilities.h"
#include <thread>

MainWindow *MainWindow::instance = nullptr;

MainWindow *MainWindow::get() {
    if (instance == nullptr) {
        instance = new MainWindow();
    }
    return instance;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    receiver(5150) {
    ui->setupUi(this);

    //startup();
    //receiver = SongStreamReceiver(5150);
    sender = SongStreamer();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    std::thread receiver(&SongStreamReceiver::init, &receiver);
    receiver.detach();
    std::thread sender(&SongStreamer::initStream, &sender, "C:\\Users\\Brody McCrone\\Desktop\\big.txt");
    sender.detach();
    //receiver.init();
    //sender.initStream("C:\\Users\\Brody McCrone\\Desktop\\test.txt");
}

void MainWindow::printToDebugLog(QString msg) {
    ui->debugLog->append(msg);
}

void MainWindow::logd(QString msg) {
    QMetaObject::invokeMethod(this, "printToDebugLog", Q_ARG(QString, msg));
}

void MainWindow::printToPacketInLog(QString msg) {
    ui->packetInLog->moveCursor(QTextCursor::End);
    ui->packetInLog->insertPlainText(msg);
}

void MainWindow::logpi(QString msg) {
    QMetaObject::invokeMethod(this, "printToPacketInLog", Q_ARG(QString, msg));
}

void MainWindow::printToPacketOutLog(QString msg) {
    ui->packetOutLog->moveCursor(QTextCursor::End);
    ui->packetOutLog->insertPlainText(msg);

}

void MainWindow::logpo(QString msg) {
    QMetaObject::invokeMethod(this, "printToPacketOutLog", Q_ARG(QString, msg));
}




void MainWindow::on_pushButton_2_clicked()
{
    QString out = ui->packetOutLog->toPlainText();
    QString in = ui->packetInLog->toPlainText();
    printToDebugLog(QString("Out size: ") + itoq(out.size()) + QString(", In size: ") + itoq(in.size()));
    if (out.compare(in) == 0) {
        ui->compRes->setText("pass");
    } else {
        ui->compRes->setText("fail");
    }
}
