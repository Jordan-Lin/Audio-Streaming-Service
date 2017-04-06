#pragma comment(lib, "ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "songstreamer.h"
#include "utilities.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);

    //startup();
    receiver = SongStreamReceiver(5150);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    receiver.init();
    SongStreamer::get().initStream("C:\\Users\\Brody McCrone\\Desktop\\test.txt");
}
