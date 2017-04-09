#ifndef CLIENTMANAGER_H
#define CLIENTMANAGER_H

#include "lockedmap.h"
#include "client.h"
#include <memory>

class ClientManager : public LockedMap<int, std::shared_ptr<Client>> {
public:
    static ClientManager& get() {return instance;};
    void addClient(Client *client);
private:
    void broadcast(WSABUF buf);
    static ClientManager& instance;
};

#endif // CLIENTMANAGER_H
