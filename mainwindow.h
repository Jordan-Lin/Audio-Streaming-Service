#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "calldialogue.h"
#include "updatehandler.h"

#include <QMainWindow>
#include <QDialog>
#include <QStringList>
#include <QStringListModel>
#include <QListView>
#include <QAbstractItemView>
#include <vector>

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
    void updatedSList(std::vector<std::string> list);
    void updatedQList(std::vector<std::string> list);
    void updatedUList(std::vector<std::string> list);


    // Button clicked event
    void on_B_Connect_clicked();
    void on_LV_SongList_doubleClicked(const QModelIndex &index);
    void on_B_Call_clicked();
    void on_B_RunServer_clicked();
    void on_B_AddItemTEST_clicked();

    void on_B_Upload_clicked();

    void on_B_Download_clicked();

    void on_B_Request_clicked();

//signals:
//    void updateSongVector(std::vector<std::string> list);
//    void updateQueueVector(std::vector<std::string> list);
//    void updateUserVector(std::vector<std::string> list);

private:
    Ui::MainWindow *ui;

    // List vectors
    std::vector<std::string> songVector;
    std::vector<std::string> queueVector;
    std::vector<std::string> userVector;

    // List Model Pairs
    QStringList songList;
    QStringListModel *SList;
    QStringList queueList;
    QStringListModel *QList;
    QStringList userList;
    QStringListModel *UList;

    // Call Dialogue Object
//    CallDialogue *callDialogue;
};

#endif // MAINWINDOW_H
