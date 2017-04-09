#ifndef CLIENTMANAGER_H
#define CLIENTMANAGER_H

#include "lockedmap.h"
#include "clienthandler.h"
#include <memory>

class ClientManager : public LockedMap<int, std::shared_ptr<ClientHandler>> {
public:
    static ClientManager& get() {return instance;}
    void addClient(ClientHandler *client);
private:
    static ClientManager instance;
    void broadcast(WSABUF buf);
};

#endif // CLIENTMANAGER_H
