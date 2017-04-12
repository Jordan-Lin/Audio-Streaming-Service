/*------------------------------------------------------------------------------
-- SOURCE FILE: mainwindow.cpp
--
-- PROGRAM: CommAudio
--
-- FUNCTIONS:
    void on_LV_SongList_doubleClicked(const QModelIndex &index);
    void on_B_Connect_clicked();
    void on_B_Call_clicked();
    void on_B_RunServer_clicked();
    void on_B_Upload_clicked();
    void on_B_Download_clicked();
    void on_B_Request_clicked();
    void disableUI();
    void on_pushButton_clicked();
    void on_LV_SongList_clicked(const QModelIndex &index);
--
-- DATE:    April 10th, 2017
--
-- DESIGNER: Jordan Lin
--
-- PROGRAMMER: Jordan Lin
--
-- NOTES: Main entry to the program. Sets up the main UI.
--
------------------------------------------------------------------------------*/
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
-- FUNCTION: MainWindow::MainWindow(QWidget *parent)
--
-- DATE:    April 9th, 2017
--
-- DESIGNER: Jordan Lin
--
-- PROGRAMMER: Jordan Lin
--
-- INTERFACE: MainWindow::MainWindow(QWidget *parent)
--
-- PARAMETERS: N/A
--
-- RETURNS: N/A
--
-- NOTES: Sets up the main interface with links to info to be updated.
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
-- FUNCTION: void MainWindow::showWarningMessage(QString title, QString msg)
--
-- DATE:    April 9th, 2017
--
-- DESIGNER: Jordan Lin
--
-- PROGRAMMER: Jordan Lin
--
-- INTERFACE: void MainWindow::showWarningMessage(QString title, QString msg)
--
-- PARAMETERS: N/A
--
-- RETURNS: N/A
--
-- NOTES: Wrapper function to show warning message box.
--
------------------------------------------------------------------------------*/
void MainWindow::showWarningMessage(QString title, QString msg)
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::information(this, title, msg);
}

/*------------------------------------------------------------------------------
-- FUNCTION: ~MainWindow()
--
-- DATE:    April 9th, 2017
--
-- DESIGNER: Jordan Lin
--
-- PROGRAMMER: Jordan Lin
--
-- INTERFACE: ~MainWindow()
--
-- PARAMETERS: N/A
--
-- RETURNS: N/A
--
-- NOTES: Destructor for MainWindow.
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
-- FUNCTION: void MainWindow::updatedSList()
--
-- DATE:    April 9th, 2017
--
-- DESIGNER: Jordan Lin
--
-- PROGRAMMER: Jordan Lin
--
-- INTERFACE: void MainWindow::updatedSList()
--
-- PARAMETERS: N/A
--
-- RETURNS: N/A
--
-- NOTES: Gets the updated song list from SongManager and updates the UI
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
-- FUNCTION: void MainWindow::updatedQList()
--
-- DATE:    April 9th, 2017
--
-- DESIGNER: Jordan Lin
--
-- PROGRAMMER: Jordan Lin
--
-- INTERFACE: void MainWindow::updatedQList()
--
-- PARAMETERS: N/A
--
-- RETURNS: N/A
--
-- NOTES: Get the updated queue of songs and update the UI.
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
-- FUNCTION: void MainWindow::updatedUList()
--
-- DATE:    April 9th, 2017
--
-- DESIGNER: Jordan Lin
--
-- PROGRAMMER: Jordan Lin
--
-- INTERFACE: void MainWindow::updatedUList()
--
-- PARAMETERS: N/A
--
-- RETURNS: N/A
--
-- NOTES: Get the updated user list and update the UI.
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
-- FUNCTION: uploadS
--
-- DATE:    April 9th, 2017
--
-- DESIGNER: Jordan Lin
--
-- PROGRAMMER: Jordan Lin
--
-- INTERFACE: view uploadS()
--
-- PARAMETERS: N/A
--
-- RETURNS: N/A
--
-- NOTES:
--  handles receiving an upload signal
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
-- FUNCTION: void MainWindow::on_B_Connect_clicked()
--
-- DATE:    April 9th, 2017
--
-- DESIGNER: Jordan Lin
--
-- PROGRAMMER: Jordan Lin
--
-- INTERFACE: void MainWindow::on_B_Connect_clicked()
--
-- PARAMETERS: N/A
--
-- RETURNS: N/A
--
-- NOTES: Get the user input of IP address and username and start the client.
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
-- FUNCTION: void MainWindow::on_LV_SongList_doubleClicked(const QModelIndex &index)
--
-- DATE:    April 9th, 2017
--
-- DESIGNER: Jordan Lin
--
-- PROGRAMMER: Jordan Lin
--
-- INTERFACE: void MainWindow::on_LV_SongList_doubleClicked(const QModelIndex &index)
--
-- PARAMETERS: N/A
--
-- RETURNS: N/A
--
-- NOTES: Clicks to send a request for a song to the server.
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

/*------------------------------------------------------------------------------
-- FUNCTION: void MainWindow::on_LV_SongList_clicked(const QModelIndex &index)
--
-- DATE:    April 9th, 2017
--
-- DESIGNER: Jordan Lin
--
-- PROGRAMMER: Jordan Lin
--
-- INTERFACE: void MainWindow::on_LV_SongList_clicked(const QModelIndex &index)
--
-- PARAMETERS: N/A
--
-- RETURNS: N/A
--
-- NOTES: Click to get the song information and update the UI.
--
------------------------------------------------------------------------------*/
void MainWindow::on_LV_SongList_clicked(const QModelIndex &index)
{
    int songSelect = index.row() + 1;
    Song curSelected = SongManager::get().at(songSelect);
    ui->L_Artist->setText(curSelected.getArtist());
    ui->L_Album->setText(curSelected.getAlbum());
}

/*------------------------------------------------------------------------------
-- FUNCTION: void MainWindow::on_B_Call_clicked()
--
-- DATE:    April 9th, 2017
--
-- DESIGNER: Jordan Lin
--
-- PROGRAMMER: Jordan Lin
--
-- INTERFACE: void MainWindow::on_B_Call_clicked()
--
-- PARAMETERS: N/A
--
-- RETURNS: N/A
--
-- NOTES: Get the user that was clicked on and get their IP to connect to.
            Start CallDialogue and establish the connection.
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
-- FUNCTION: void MainWindow::on_B_RunServer_clicked()
--
-- DATE:    April 9th, 2017
--
-- DESIGNER: Jordan Lin
--
-- PROGRAMMER: Jordan Lin
--
-- INTERFACE: void MainWindow::on_B_RunServer_clicked()
--
-- PARAMETERS: N/A
--
-- RETURNS: N/A
--
-- NOTES: Click on to establish itself as server and disable other fields in the UI
            used by clients.
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
-- FUNCTION: void MainWindow::on_B_Upload_clicked()
--
-- DATE:    April 9th, 2017
--
-- DESIGNER: Jordan Lin
--
-- PROGRAMMER: Jordan Lin
--
-- INTERFACE: void MainWindow::on_B_Upload_clicked()
--
-- PARAMETERS: N/A
--
-- RETURNS: N/A
--
-- NOTES: Show the file select dialogue for upload songs.
--
------------------------------------------------------------------------------*/
void MainWindow::on_B_Upload_clicked()
{
    FileSelectDialogue::get()->show();
}

/*------------------------------------------------------------------------------
-- FUNCTION: void MainWindow::on_B_Download_clicked()
--
-- DATE:    April 9th, 2017
--
-- DESIGNER: Jordan Lin
--
-- PROGRAMMER: Jordan Lin
--
-- INTERFACE: void MainWindow::on_B_Download_clicked()
--
-- PARAMETERS: N/A
--
-- RETURNS: N/A
--
-- NOTES: Request to download the selected song from the server.
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
-- FUNCTION: void MainWindow::on_B_Request_clicked()
--
-- DATE:    April 9th, 2017
--
-- DESIGNER: Jordan Lin
--
-- PROGRAMMER: Jordan Lin
--
-- INTERFACE: void MainWindow::on_B_Request_clicked()
--
-- PARAMETERS: N/A
--
-- RETURNS: N/A
--
-- NOTES: Create a song request for the song that is currently selected.
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
-- FUNCTION: void MainWindow::disableUI()
--
-- DATE:    April 9th, 2017
--
-- DESIGNER: Jordan Lin
--
-- PROGRAMMER: Jordan Lin
--
-- INTERFACE: void MainWindow::disableUI()
--
-- PARAMETERS: N/A
--
-- RETURNS: N/A
--
-- NOTES: Function to disable the UI
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
