#include "clientmanager.h"
#include "utilities.h"
#include "debugwindow.h"

ClientManager ClientManager::instance;

void ClientManager::addClient(ClientHandler *client) {
    std::lock_guard<std::mutex> guard(lock);
    map.insert(std::pair<int, std::shared_ptr<ClientHandler>>
               (client->getSocket(), std::shared_ptr<ClientHandler>(client)));
    unlockedSendUserList();
}

void ClientManager::removeClient(int id) {
    std::lock_guard<std::mutex> guard(lock);
    map.erase(id);

    unlockedSendUserList();
}

void ClientManager::broadcast(WSABUF buf) {
    std::lock_guard<std::mutex> guard(lock);
    unlockedBroadcast(buf);
}

void ClientManager::unlockedBroadcast(WSABUF buf) {
    for (auto elem : map) {
        sendTCP(elem.first, buf);
    }
}

void ClientManager::sendUserList() {
    std::lock_guard<std::mutex> guard(lock);
    unlockedSendUserList();
}

void ClientManager::unlockedSendUserList() {
    int numClients = map.size();
    PktIds pktId = PktIds::USERS;
    WSABUF wsaBuf;
    wsaBuf.buf = buffer;

    wsaBuf.len = static_cast<u_long>(numClients) * sizeof(UserInfo) + sizeof(int) + sizeof(pktId);

    memcpy(wsaBuf.buf, &pktId, sizeof(pktId));
    int offset = sizeof(pktId);

    memcpy(wsaBuf.buf + offset, &numClients, sizeof(int));
    offset += sizeof(int);

    for (auto& client : map) {
        DebugWindow::get()->logpo(QString("<UserInfo> id: ") +
            itoq(client.second.get()->getInfo().userId) +
            QString(", username: ") +
            QString(client.second.get()->getInfo().username) +
            QString(", ip: ") +
            QString(itoq(client.second.get()->getInfo().ip)) +
            QString("\n"));
        memcpy(wsaBuf.buf + offset, &client.second.get()->getInfo(), sizeof(UserInfo));
        offset += sizeof(UserInfo);
    }

    unlockedBroadcast(wsaBuf);
}
