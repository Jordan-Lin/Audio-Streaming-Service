#include "clientmanager.h"

void ClientManager::addClient(Client *client) {
    std::lock_guard<std::mutex> guard(lock);
    WSABUF buf;
    buf.len = (map.size() + 1) * sizeof(UserInfo) + sizeof(PktIds::USERS)
            + sizeof(int);
    buf.buf = (char *)malloc(buf.len);
    int offset = 0;
    for (auto elem : map) {
        UserInfo info = elem.second.get().getInfo();
        memcpy(buffer + offset, &info, sizeof(UserInfo));
        offset += sizeof(UserInfo);
    }
    wsaBuf.len = offset;
    sendTCP(client->getSocket(), buf);
    memcpy(buffer + offset, client->getInfo(), offset);
    offset += sizeof(UserInfo);
    wsaBuf.len = offset;
    broadcast(buf);
    map.insert(std::pair<int, std::shared_ptr<Client>>
               (client->getSocket(), std::shared_ptr<Client>(client));
    free(buffer);
}

void ClientManager::broadcast(WSABUF buf) {
    for (auto elem : map) {
        sendTCP(elem.first, buffer, len);
    }
}
