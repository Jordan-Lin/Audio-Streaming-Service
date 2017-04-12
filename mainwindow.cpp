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

    ui->HS_SongProgress->setEnabled(false);

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
    if (server != nullptr) {
        delete server;
    }
    if (client != nullptr) {
        delete client;
    }
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
    /*
    songVector.push_back("Song");
    UpdateHandler::get()->emitUSV(songVector);
    queueVector.push_back("Song");
    UpdateHandler::get()->emitUQV(queueVector);
    userVector.push_back("User");
    UpdateHandler::get()->emitUUV(userVector);
    */
    //User u("UserTest", 123);
    //UserManager::get().insert(123, u);
    //UpdateHandler::get()->emitUUV();
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
    qDebug() << "Selectd song index = "<< song;
    // Grab song from map using the row, get the song information to place a request.
    // Send Song request
    // Wait for update received from server to add queue'd song to list.
    if (client != nullptr) {
        client->sendSongRequest(song);
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

    const u_long ip = UserManager::get().at(rowToUserId[userSelect]).getIp();
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
    ui->L_SongName->setText("Running as server");
    ui->L_SongTime->setText("Running as server");
}
/*----------------------------------------------
 UI Manipulation
----------------------------------------------*/

void MainWindow::on_pushButton_clicked()
{
    QByteArray data = audioManager::get().loadHeader(SongManager::get().at(1).getDir());
    HeaderInfo info = audioManager::get().parseHeader(data);
    data = audioManager::get().loadAudio(SongManager::get().at(1).getDir());
    audioManager::get().appender(data);
    std::thread audioThread(&audioManager::initAudio, &audioManager::get(), info.bitsPerSample, info.sampleRate, info.numberOfChannels);
    audioThread.detach();
    audioManager::get().playSong();
}
