#include "clientmanager.h"
#include "utilities.h"

ClientManager ClientManager::instance;

void ClientManager::addClient(ClientHandler *client) {
    std::lock_guard<std::mutex> guard(lock);
    int numUsers = map.size() + 1;
    std::vector<char> data;
    data.resize(static_cast<u_long>((numUsers) * sizeof(UserInfo)) + sizeof(PktIds::USERS)
                + sizeof(int));
    PktIds pktId = PktIds::USERS;
    WSABUF wsaBuf;
    wsaBuf.len = static_cast<u_long>((numUsers) * sizeof(UserInfo)) + sizeof(PktIds::USERS)
            + sizeof(int);
    wsaBuf.buf = data.data();
    memcpy(wsaBuf.buf, reinterpret_cast<char *>(&pktId), sizeof(PktIds::USERS));
    int offset = sizeof(PktIds::USERS);
    memcpy(wsaBuf.buf + offset, reinterpret_cast<char *>(&(--numUsers)), sizeof(int));
    offset += sizeof(int);
    for (auto elem : map) {
        UserInfo info = elem.second.get()->getInfo();
        memcpy(wsaBuf.buf + offset, &info, sizeof(UserInfo));
        offset += sizeof(UserInfo);
    }
    wsaBuf.len = offset;
    sendTCP(client->getSocket(), wsaBuf);
    memcpy(wsaBuf.buf + sizeof(PktIds::USERS), reinterpret_cast<char *>(&(++numUsers)), sizeof(int));
    memcpy(wsaBuf.buf + offset, &client->getInfo(), sizeof(UserInfo));
    offset += sizeof(UserInfo);
    wsaBuf.len = offset;
    broadcast(wsaBuf);
    map.insert(std::pair<int, std::shared_ptr<ClientHandler>>
               (client->getSocket(), std::shared_ptr<ClientHandler>(client)));
}

void ClientManager::broadcast(WSABUF buf) {
    for (auto elem : map) {
        sendTCP(elem.first, buf);
    }
}
