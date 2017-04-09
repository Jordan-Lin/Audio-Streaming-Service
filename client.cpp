#include "client.h"

#include <thread>
#include "utilities.h"
#include <memory>

Client(SOCKET sock)
        : sock(sock) {
    info.userId = sock;
    std::thread clientThread(Client::init, this);
    clientThread.detach();
}

void Client::init() {
    receive(sock, wsaBuf, olapWrap.olap, receiveRoutine);
}

void CALLBACK Client::receiveRoutine(DWORD errCode, DWORD recvBytes, LPOVERLAPPED olap, DWORD flags) {
    if (errCode != 0) {
        MainWindow::get()->logd("Client::receiveRoutine failed.");
    }

    reinterpret_cast<ClientOlap *>(olap)->client->parse(recvBytes);
    receive(sock, wsaBuf, olapWrap.olap, receiveRoutine);
}

void Client::parse(int recvBytes) {
    PktIds *pktId = reinterpret_cast<PktIds *>(buffer);
    while (recvBytes) {
        switch (*pktId) {
        case PktIds::JOIN:
        {
            Join *join = reinterpret_cast<Join *>(buffer);
            memcpy(info.username, join->username, strlen(join->username));
            ClientManager::get().addClient(this);
            recvBytes -= sizeof(Join);
        }
    }
}
