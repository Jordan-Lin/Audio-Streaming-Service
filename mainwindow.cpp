#pragma comment(lib, "ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QT>
#include <QDebug>
#include <QMessageBox>
#include <QAbstractItemView>

#include <string>
#include <ws2tcpip.h>

/*------------------------------------------------------------------------------
-- FUNCTION:
--
-- DATE:    April 9th, 2017
--
-- DESIGNER: Jordan Lin
--
-- PROGRAMMER: Jordan Lin
--
-- INTERFACE:
--
-- PARAMETERS: N/A
--
-- RETURNS: N/A
--
-- NOTES:
--
------------------------------------------------------------------------------*/
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFixedSize(this->size());

    // Remove editing lists manually
    ui->LV_SongList->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->LV_QueueList->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->LV_QueueList->setSelectionMode(QAbstractItemView::NoSelection);
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
//    connect(this, &MainWindow::updateSongVector, this, &MainWindow::updatedSList);
//    connect(this, &MainWindow::updateQueueVector, this, &MainWindow::updatedQList);
//    connect(this, &MainWindow::updateUserVector, this, &MainWindow::updatedUList);

}

/*----------------------------------------------
 QT Wrapper Functions
----------------------------------------------*/
/*------------------------------------------------------------------------------
-- FUNCTION:
--
-- DATE:    April 9th, 2017
--
-- DESIGNER: Jordan Lin
--
-- PROGRAMMER: Jordan Lin
--
-- INTERFACE:
--
-- PARAMETERS: N/A
--
-- RETURNS: N/A
--
-- NOTES:
--
------------------------------------------------------------------------------*/
void MainWindow::showWarningMessage(QString title, QString msg)
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::information(this, title, msg);
//    if (reply == QMessageBox::Yes) {
//      qDebug() << "Yes was clicked";
//    } else {
//      qDebug() << "Yes was *not* clicked";
//    }
}

/*------------------------------------------------------------------------------
-- FUNCTION:
--
-- DATE:    April 9th, 2017
--
-- DESIGNER: Jordan Lin
--
-- PROGRAMMER: Jordan Lin
--
-- INTERFACE:
--
-- PARAMETERS: N/A
--
-- RETURNS: N/A
--
-- NOTES:
--
------------------------------------------------------------------------------*/
MainWindow::~MainWindow()
{
    delete ui;
}



/*----------------------------------------------
 Test Buttons
----------------------------------------------*/
/*------------------------------------------------------------------------------
-- FUNCTION:
--
-- DATE:    April 9th, 2017
--
-- DESIGNER: Jordan Lin
--
-- PROGRAMMER: Jordan Lin
--
-- INTERFACE:
--
-- PARAMETERS: N/A
--
-- RETURNS: N/A
--
-- NOTES:
--
------------------------------------------------------------------------------*/
void MainWindow::on_B_AddItemTEST_clicked()
{
    songVector.push_back("Song");
    UpdateHandler::get()->emitUSV(songVector);
    queueVector.push_back("Song");
    UpdateHandler::get()->emitUQV(queueVector);
    userVector.push_back("User");
    UpdateHandler::get()->emitUUV(userVector);
}
/*----------------------------------------------
 Test Buttons
----------------------------------------------*/


/*----------------------------------------------
 Update Slots
----------------------------------------------*/
/*------------------------------------------------------------------------------
-- FUNCTION:
--
-- DATE:    April 9th, 2017
--
-- DESIGNER: Jordan Lin
--
-- PROGRAMMER: Jordan Lin
--
-- INTERFACE:
--
-- PARAMETERS: N/A
--
-- RETURNS: N/A
--
-- NOTES:
--
------------------------------------------------------------------------------*/
void MainWindow::updatedSList(std::vector<std::string> list)
{
    songList.clear();
    for (const auto& i : list) {
        songList << QString::fromStdString(i);
    }
    SList->setStringList(songList);
    ui->LV_SongList->setModel(SList);
}

/*------------------------------------------------------------------------------
-- FUNCTION:
--
-- DATE:    April 9th, 2017
--
-- DESIGNER: Jordan Lin
--
-- PROGRAMMER: Jordan Lin
--
-- INTERFACE:
--
-- PARAMETERS: N/A
--
-- RETURNS: N/A
--
-- NOTES:
--
------------------------------------------------------------------------------*/
void MainWindow::updatedQList(std::vector<std::string> list)
{
    queueList.clear();

    for (const auto& i : list) {
        queueList << QString::fromStdString(i);
    }
    this->QList->setStringList(queueList);
    ui->LV_QueueList->setModel(this->QList);
}

/*------------------------------------------------------------------------------
-- FUNCTION:
--
-- DATE:    April 9th, 2017
--
-- DESIGNER: Jordan Lin
--
-- PROGRAMMER: Jordan Lin
--
-- INTERFACE:
--
-- PARAMETERS: N/A
--
-- RETURNS: N/A
--
-- NOTES:
--
------------------------------------------------------------------------------*/
void MainWindow::updatedUList(std::vector<std::string> list)
{
    userList.clear();
    for (const auto& i : list) {
        userList << QString::fromStdString(i);
    }
    UList->setStringList(userList);
    ui->LV_UserList->setModel(UList);
}
/*----------------------------------------------
 Update Slots
----------------------------------------------*/


/*----------------------------------------------
 Button press slots
----------------------------------------------*/
/*------------------------------------------------------------------------------
-- FUNCTION:
--
-- DATE:    April 9th, 2017
--
-- DESIGNER: Jordan Lin
--
-- PROGRAMMER: Jordan Lin
--
-- INTERFACE:
--
-- PARAMETERS: N/A
--
-- RETURNS: N/A
--
-- NOTES:
--
------------------------------------------------------------------------------*/
void MainWindow::on_B_Connect_clicked()
{
    if(!(ui->LE_IPAddress->text().isEmpty()) && !(ui->LE_Username->text().isEmpty()))
    {
        std::string tip  = ui->LE_IPAddress->text().toStdString();
//        int port  = ui->LE_Port->text().toInt();                              // Remove cause port is unneeded. Leave if changed mind
        u_long ip = stoul(tip, nullptr, 0);
        // Utilities call for creating socket structure.
        // Use the socket structure for connecting to the server.
    } else {
        showWarningMessage("Missing Input", "Please enter a username and IP address.");
    }
}

/*------------------------------------------------------------------------------
-- FUNCTION:
--
-- DATE:    April 9th, 2017
--
-- DESIGNER: Jordan Lin
--
-- PROGRAMMER: Jordan Lin
--
-- INTERFACE:
--
-- PARAMETERS: N/A
--
-- RETURNS: N/A
--
-- NOTES:
--
------------------------------------------------------------------------------*/
void MainWindow::on_LV_SongList_doubleClicked(const QModelIndex &index)
{
    std::string songSelect = index.data(Qt::DisplayRole).toString().toStdString();
    int song = index.row();
    qDebug() << "Selectd song index = "<< song;
    // Grab song from map using the row, get the song information to place a request.
    // Send Song request
    // Wait for update received from server to add queue'd song to list.
}

/*------------------------------------------------------------------------------
-- FUNCTION:
--
-- DATE:    April 9th, 2017
--
-- DESIGNER: Jordan Lin
--
-- PROGRAMMER: Jordan Lin
--
-- INTERFACE:
--
-- PARAMETERS: N/A
--
-- RETURNS: N/A
--
-- NOTES:
--
------------------------------------------------------------------------------*/
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

/*------------------------------------------------------------------------------
-- FUNCTION:
--
-- DATE:    April 9th, 2017
--
-- DESIGNER: Jordan Lin
--
-- PROGRAMMER: Jordan Lin
--
-- INTERFACE:
--
-- PARAMETERS: N/A
--
-- RETURNS: N/A
--
-- NOTES:
--
------------------------------------------------------------------------------*/
void MainWindow::on_B_RunServer_clicked()
{
    // TODO Set all buttons, Horizontal slider, line edits and list views to disabled.
}

/*------------------------------------------------------------------------------
-- FUNCTION:
--
-- DATE:    April 9th, 2017
--
-- DESIGNER: Jordan Lin
--
-- PROGRAMMER: Jordan Lin
--
-- INTERFACE:
--
-- PARAMETERS: N/A
--
-- RETURNS: N/A
--
-- NOTES:
--
------------------------------------------------------------------------------*/
void MainWindow::on_B_Upload_clicked()
{
    // Open file select dialogue, get path, open file, write to buffer and send to server for upload.
}

/*------------------------------------------------------------------------------
-- FUNCTION:
--
-- DATE:    April 9th, 2017
--
-- DESIGNER: Jordan Lin
--
-- PROGRAMMER: Jordan Lin
--
-- INTERFACE:
--
-- PARAMETERS: N/A
--
-- RETURNS: N/A
--
-- NOTES:
--
------------------------------------------------------------------------------*/
void MainWindow::on_B_Download_clicked()
{
    // Get Selected item from SongList, send request with song info packet, run receive loop till all packets received.
    // Loop should happen in completion routine.
}

/*------------------------------------------------------------------------------
-- FUNCTION:
--
-- DATE:    April 9th, 2017
--
-- DESIGNER: Jordan Lin
--
-- PROGRAMMER: Jordan Lin
--
-- INTERFACE:
--
-- PARAMETERS: N/A
--
-- RETURNS: N/A
--
-- NOTES:
--
------------------------------------------------------------------------------*/
void MainWindow::on_B_Request_clicked()
{
    // Get Selected item from SongList
    // Create song request packet
    // Send some request packet.
}

/*----------------------------------------------
 Button press slots
----------------------------------------------*/
