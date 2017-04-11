#ifndef CLIENTMANAGER_H
#define CLIENTMANAGER_H

#include "lockedmap.h"
#include "clienthandler.h"
#include <memory>
#include "defines.h"

class ClientManager : public LockedMap<int, std::shared_ptr<ClientHandler>> {
public:
    static ClientManager& get() {return instance;}
    void addClient(ClientHandler *client);
    void removeClient(int id);
    void broadcast(WSABUF buf);
    void sendUserList();
private:
    static ClientManager instance;
    char buffer[LARGE_BUFFER_SIZE];

    void unlockedBroadcast(WSABUF wsaBuf);
    void unlockedSendUserList();
};

#endif // CLIENTMANAGER_H
