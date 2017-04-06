#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "songstreamreceiver.h"
#include "songstreamer.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    static MainWindow *instance;
    static MainWindow *get();
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void logd(QString msg);
    void logpo(QString msg);
    void logpi(QString msg);
private slots:
    void on_pushButton_clicked();
    void printToDebugLog(QString msg);
    void printToPacketOutLog(QString msg);
    void printToPacketInLog(QString msg);
    void on_pushButton_2_clicked();

private:
    Ui::MainWindow *ui;
    SongStreamReceiver receiver;
    SongStreamer sender;
};

#endif // MAINWINDOW_H
