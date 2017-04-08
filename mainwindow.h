#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
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

private slots:
    // Button clicked event
    void on_TestAddList_clicked();
    void on_TestRemoveList_clicked();
    void on_TestProgressBar_clicked();

    // Update list slots
    void updatedSList(std::vector<std::string> list);
    void updatedQList(std::vector<std::string> list);
    void updatedUList(std::vector<std::string> list);

    void on_B_Connect_clicked();

    void on_LV_SongList_doubleClicked(const QModelIndex &index);

signals:
    void updateSongVector(std::vector<std::string> list);
    void updateQueueVector(std::vector<std::string> list);
    void updateUserVector(std::vector<std::string> list);

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
};

#endif // MAINWINDOW_H
