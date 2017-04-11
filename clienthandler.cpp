#include "clienthandler.h"

#include <thread>
#include "utilities.h"
#include <memory>
#include "debugwindow.h"
#include "clientmanager.h"
#include "songmanager.h"
#include "songqueue.h"

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
        DebugWindow::get()->logd("Client::receiveRoutine failed.");
    }

    ClientHandler *client = reinterpret_cast<ClientHandlerOlap *>(olap)->client;
    client->handleReceive(recvBytes);
}

void ClientHandler::handleReceive(int recvBytes) {
    if (recvBytes == 0) {
        DebugWindow::get()->logd(QString("Client disconnected: ") + QString(info.username) + itoq(info.userId));
        ClientManager::get().removeClient(info.userId);
    } else {
        parse(recvBytes);
    }
}

void ClientHandler::parse(int recvBytes) {
    PktIds *pktId = reinterpret_cast<PktIds *>(buffer);
    while (recvBytes) {
        switch (*pktId) {
        case PktIds::JOIN:
            {
                Join *join = reinterpret_cast<Join *>(buffer);
                DebugWindow::get()->logd(QString("THIS PERSON JOINED: ") + join->username);

                UserId idPkt;
                idPkt.id = info.userId;
                WSABUF wsaBuf;
                wsaBuf.buf = reinterpret_cast<char *>(&idPkt);
                wsaBuf.len = sizeof(idPkt);
                sendTCP(info.userId, wsaBuf);

                memcpy(info.username, join->username, strlen(join->username) + 1);
                ClientManager::get().addClient(this);
                recvBytes -= sizeof(Join);

                SongManager::get().sendSongList();
                SongQueue::get().sendSongQueue();
            }
            break;
        case PktIds::SONG_REQUEST:
            {
                SongRequest *request = reinterpret_cast<SongRequest *>(buffer);
                SongQueue::get().addSong(SongManager::get().at(request->songId));

                recvBytes -= sizeof(SongRequest);
            }
            break;
        }
    }

    wsaBuf.buf = buffer;
    wsaBuf.len = sizeof(buffer);
    receive(info.userId, wsaBuf, &olapWrap.olap, receiveRoutine);
}
