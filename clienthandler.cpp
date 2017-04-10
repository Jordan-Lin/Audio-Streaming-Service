#include "clienthandler.h"

#include <thread>
#include "utilities.h"
#include <memory>
#include "mainwindow.h"
#include "clientmanager.h"

ClientHandler::ClientHandler(SOCKET sock) {
    info.userId = sock;
    olapWrap.client = this;
    std::thread clientThread(&ClientHandler::init, this);
    clientThread.detach();
}

void ClientHandler::init() {
    wsaBuf.buf = buffer;
    wsaBuf.len = sizeof(buffer);
    receive(info.userId, wsaBuf, &olapWrap.olap, receiveRoutine);
    while (true) {
        SleepEx(INFINITE, TRUE);
    }
}

void CALLBACK ClientHandler::receiveRoutine(DWORD errCode, DWORD recvBytes, LPOVERLAPPED olap, DWORD flags) {
    if (errCode != 0) {
        MainWindow::get()->logd("Client::receiveRoutine failed.");
    }

    ClientHandler *client = reinterpret_cast<ClientHandlerOlap *>(olap)->client;
    client->parse(recvBytes);
}

void ClientHandler::parse(int recvBytes) {
    PktIds *pktId = reinterpret_cast<PktIds *>(buffer);
    while (recvBytes) {
        switch (*pktId) {
        case PktIds::JOIN:
            {
                Join *join = reinterpret_cast<Join *>(buffer);
                MainWindow::get()->logd(QString("THIS PERSON JOINED: ") + join->username);
                memcpy(info.username, join->username, strlen(join->username) + 1);
                ClientManager::get().addClient(this);
                recvBytes -= sizeof(Join);
            }
        }
    }

    wsaBuf.buf = buffer;
    wsaBuf.len = sizeof(buffer);
    receive(info.userId, wsaBuf, &olapWrap.olap, receiveRoutine);
}
