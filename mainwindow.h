#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "calldialogue.h"
#include "updatehandler.h"
#include "server.h"
#include "client.h"

#include <QMainWindow>
#include <QDialog>
#include <QStringList>
#include <QStringListModel>
#include <QListView>
#include <QAbstractItemView>
#include <vector>
#include <map>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void updateSongList();

    void showWarningMessage(QString type, QString msg);

private slots:
    // Update list slots
    void updatedSList();
    void updatedQList();
    void updatedUList();


    // Button clicked event
    void on_LV_SongList_doubleClicked(const QModelIndex &index);
    void on_B_Connect_clicked();
    void on_B_Call_clicked();
    void on_B_RunServer_clicked();
    void on_B_AddItemTEST_clicked();
    void on_B_Upload_clicked();
    void on_B_Download_clicked();
    void on_B_Request_clicked();

    // UI manipulation
    void disableUI();

//signals:
//    void updateSongVector(std::vector<std::string> list);
//    void updateQueueVector(std::vector<std::string> list);
//    void updateUserVector(std::vector<std::string> list);

private:
    Ui::MainWindow *ui;

    // List Model Pairs
    std::map<int, int> rowToUserId;
    QStringList songList;
    QStringListModel *SList;
    QStringList queueList;
    QStringListModel *QList;
    QStringList userList;
    QStringListModel *UList;

    Server *server = nullptr;
    Client *client = nullptr;
};

#endif // MAINWINDOW_H
