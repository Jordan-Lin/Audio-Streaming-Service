#pragma comment(lib, "ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "fileselectdialogue.h"

#include <QT>
#include <QDir>
#include <QDebug>
#include <QMessageBox>
#include <QAbstractItemView>
#include "usermanager.h"
#include "songmanager.h"
#include "debugwindow.h"
#include "songqueue.h"
#include "audiomanager.h"

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
    if (server != nullptr) {
        delete server;
    }
    if (client != nullptr) {
        delete client;
    }
}


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
void MainWindow::updatedSList()
{
    songList.clear();
    for (const auto& song : SongManager::get().getAll()) {
        songList << song.getTitle();
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
void MainWindow::updatedQList()
{
    queueList.clear();

    for (const auto& song : SongQueue::get().getAll()) {
        queueList << song.getTitle();
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
void MainWindow::updatedUList()
{
    DebugWindow::get()->logd("  ");
    userList.clear();
    rowToUserId.clear();
    int row = 0;
    for (const auto& user : UserManager::get().getAll()) {
        rowToUserId[row] = user.getId();
        row++;
        userList << user.getUsername();
    }
    UList->setStringList(userList);
    ui->LV_UserList->setModel(UList);
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
void MainWindow::uploadS() {
    QString title = FileSelectDialogue::get()->getTitle();
    QString album = FileSelectDialogue::get()->getAlbum();
    QString artist = FileSelectDialogue::get()->getArtist();
    QString filePath = FileSelectDialogue::get()->getFilePath();
    client->sendUploadRequest(title, album, artist, filePath);
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
        if (client == nullptr) {
            client = new Client (ui->LE_IPAddress->text(), ui->LE_Username->text());
        }
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
    if (client != nullptr) {
        client->sendSongRequest(song);
    }
}



void MainWindow::on_LV_SongList_clicked(const QModelIndex &index)
{
    int songSelect = index.row() + 1;
    Song curSelected = SongManager::get().at(songSelect);
    ui->L_Artist->setText(curSelected.getArtist());
    ui->L_Album->setText(curSelected.getAlbum());
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
    // Select user from the list based on index id.
    int userSelect;
    if((userSelect = ui->LV_UserList->currentIndex().row()) == -1) {
        return;
    }
    QModelIndex t = ui->LV_UserList->currentIndex();
    std::string uName = t.data(0).toString().toStdString();

    const u_long ip = UserManager::get().at(rowToUserId[userSelect]).getIp();
    CallDialogue::get()->setContact(uName);
    CallDialogue::get()->setIp(ip);
    CallDialogue::get()->show();
    CallDialogue::get()->init();
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
    // Change UI Settings, disable unneeded sections for server.
    disableUI();

    // Get all files in song folder
    SongManager::get().LoadSongList();

    // Initialize the server.
    if (server == nullptr) {
        server = new Server();
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
void MainWindow::on_B_Upload_clicked()
{
    FileSelectDialogue::get()->show();
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
    if (client != nullptr) {
        int userSelection = ui->LV_SongList->currentIndex().row();
        client->sendDownloadRequest(userSelection);
    }
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

/*----------------------------------------------
 UI Manipulation
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
void MainWindow::disableUI() {
    // Change UI Settings, disable unneeded sections for server.
    ui->B_Call->setDisabled(true);
    ui->B_Request->setDisabled(true);
    ui->B_Download->setDisabled(true);
    ui->B_Upload->setDisabled(true);
    ui->B_Connect->setDisabled(true);
    ui->L_Album->hide();
    ui->L_Artist->hide();
    ui->Artist->hide();
    ui->Album->hide();
    QWidget::setWindowTitle("Server");
}
/*----------------------------------------------
 UI Manipulation
----------------------------------------------*/
