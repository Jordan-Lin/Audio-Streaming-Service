#pragma comment(lib, "ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QT>
#include <QDebug>
#include <ws2tcpip.h>
#include <string>
#include <QAbstractItemView>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFixedSize(this->size());

    // Remove editing lists manually
    ui->LV_SongList->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->LV_QueueList->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->LV_UserList->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // Create model
    SList = new QStringListModel(this);
    QList = new QStringListModel(this);
    UList = new QStringListModel(this);

    // Attach String Lists to Models
    SList->setStringList(songList);
    QList->setStringList(queueList);
    UList->setStringList(userList);

    // Glue models and views together
    ui->LV_SongList->setModel(SList);
    ui->LV_QueueList->setModel(QList);
    ui->LV_UserList->setModel(UList);

    // Connect signals to slots for Lists
    connect(this, &MainWindow::updateSongVector, this, &MainWindow::updatedSList);
    connect(this, &MainWindow::updateQueueVector, this, &MainWindow::updatedQList);
    connect(this, &MainWindow::updateUserVector, this, &MainWindow::updatedUList);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_TestAddList_clicked()
{
    songVector.push_back("Song");
    emit updateSongVector(songVector);
}

void MainWindow::on_TestRemoveList_clicked()
{
    queueVector.push_back("Song");
    emit updateQueueVector(queueVector);
}

void MainWindow::on_TestProgressBar_clicked()
{
    userVector.push_back("User");
    emit updateUserVector(userVector);
}

void MainWindow::updatedSList(std::vector<std::string> list)
{
    songList.clear();
    for (const auto& i : list) {
        songList << QString::fromStdString(i);
    }
    SList->setStringList(songList);
    ui->LV_SongList->setModel(SList);
}

void MainWindow::updatedQList(std::vector<std::string> list)
{
    queueList.clear();

    for (const auto& i : list) {
        queueList << QString::fromStdString(i);
    }
    this->QList->setStringList(queueList);
    ui->LV_QueueList->setModel(this->QList);
}

void MainWindow::updatedUList(std::vector<std::string> list)
{
    userList.clear();
    for (const auto& i : list) {
        userList << QString::fromStdString(i);
    }
    UList->setStringList(userList);
    ui->LV_UserList->setModel(UList);
}

void MainWindow::on_B_Connect_clicked()
{
    if(!(ui->LE_IPAddress->text().isEmpty()) || !(ui->LE_Port->text().isEmpty()))
    {
        std::string tip  = ui->LE_IPAddress->text().toStdString();
        int port  = ui->LE_Port->text().toInt();
        u_long ip = stoul(tip, nullptr, 0);
        // Utilities call for creating socket structure.
        // Use the socket structure for connecting to the server.
    }
}

void MainWindow::on_LV_SongList_doubleClicked(const QModelIndex &index)
{
    std::string songSelect = index.data(Qt::DisplayRole).toString().toStdString();
    int song = index.row();
    // Grab song from map using the row, get the song information to place a request.
    // Send Song request
    // Wait for update received from server to add queue'd song to list.
}

void MainWindow::on_B_Call_clicked()
{
    int userSelect;
    if((userSelect = ui->LV_UserList->currentIndex().row()) == -1) {
        return;
    }
    QModelIndex t = ui->LV_UserList->currentIndex();
    std::string uName = t.data(0).toString().toStdString();
    // TODO: get the information from the index selected of the Map or vector or list to get user information.

    // Get user list from map of name and struct.
    qDebug() << "selected = " << userSelect;

    CallDialogue::get()->show();
    CallDialogue::get()->setContact(uName);
    CallDialogue::get()->init();
//    CallDialogue::get()->print(CallDialogue::get()->getContact());
}
