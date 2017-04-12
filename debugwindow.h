#ifndef DebugWindow_H
#define DebugWindow_H

#include "ui_debugwindow.h"
#include <QMainWindow>

namespace Ui {
class DebugWindow;
}

class DebugWindow : public QMainWindow
{
    Q_OBJECT

public:
    static DebugWindow *instance;
    static DebugWindow *get();
    explicit DebugWindow(QWidget *parent = 0);
    ~DebugWindow();

    void logd(QString msg);
    void logpo(QString msg);
    void logpi(QString msg);
private slots:
    void printToDebugLog(QString msg);
    void printToPacketOutLog(QString msg);
    void printToPacketInLog(QString msg);

private:
    Ui::DebugWindow *ui;

};

#endif // DebugWindow_H
