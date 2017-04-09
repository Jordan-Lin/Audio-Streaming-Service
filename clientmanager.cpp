#include "clientmanager.h"
#include "utilities.h"

ClientManager ClientManager::instance;

void ClientManager::addClient(ClientHandler *client) {
    std::lock_guard<std::mutex> guard(lock);
    WSABUF wsaBuf;
    wsaBuf.len = static_cast<u_long>((map.size() + 1)) * sizeof(UserInfo) + sizeof(PktIds::USERS)
            + sizeof(int);
    wsaBuf.buf = (char *)malloc(wsaBuf.len);
    int offset = 0;
    for (auto elem : map) {
        UserInfo info = elem.second.get()->getInfo();
        memcpy(wsaBuf.buf + offset, &info, sizeof(UserInfo));
        offset += sizeof(UserInfo);
    }
    wsaBuf.len = offset;
    sendTCP(client->getSocket(), wsaBuf);
    memcpy(wsaBuf.buf + offset, &client->getInfo(), offset);
    offset += sizeof(UserInfo);
    wsaBuf.len = offset;
    broadcast(wsaBuf);
    map.insert(std::pair<int, std::shared_ptr<ClientHandler>>
               (client->getSocket(), std::shared_ptr<ClientHandler>(client)));
    free(wsaBuf.buf);
}

void ClientManager::broadcast(WSABUF buf) {
    for (auto elem : map) {
        sendTCP(elem.first, buf);
    }
}
