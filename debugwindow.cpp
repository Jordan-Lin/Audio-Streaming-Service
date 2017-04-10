#pragma comment(lib, "ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "debugwindow.h"
#include "ui_debugwindow.h"
#include "songstreamer.h"
#include "utilities.h"
#include <thread>
#include <QFileDialog>
#include "client.h"
#include "server.h"


DebugWindow *DebugWindow::instance = nullptr;

DebugWindow *DebugWindow::get() {
    if (instance == nullptr) {
        instance = new DebugWindow();
    }
    return instance;
}

DebugWindow::DebugWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DebugWindow)
{
    ui->setupUi(this);
}

DebugWindow::~DebugWindow()
{
    delete ui;
}

void DebugWindow::printToDebugLog(QString msg) {
    ui->TB_DebugLog->append(msg);
}

void DebugWindow::logd(QString msg) {
    QMetaObject::invokeMethod(this, "printToDebugLog", Q_ARG(QString, msg));
}

void DebugWindow::printToPacketInLog(QString msg) {
    ui->TB_PacketIn->moveCursor(QTextCursor::End);
    ui->TB_PacketIn->insertPlainText(msg);
}

void DebugWindow::logpi(QString msg) {
    QMetaObject::invokeMethod(this, "printToPacketInLog", Q_ARG(QString, msg));
}

void DebugWindow::printToPacketOutLog(QString msg) {
    ui->TB_PacketOut->moveCursor(QTextCursor::End);
    ui->TB_PacketOut->insertPlainText(msg);
}

void DebugWindow::logpo(QString msg) {
    QMetaObject::invokeMethod(this, "printToPacketOutLog", Q_ARG(QString, msg));
}
