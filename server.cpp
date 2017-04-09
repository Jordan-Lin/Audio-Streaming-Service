#include "server.h"
#include "client.h"
#include "clientmanager.h"
#include <thread>

Server::Server() {
    std::thread(this, &Server::run);
}

void run() {
    listenSock = createSocket(SOCK_STREAM);
    bindSocket(listenSock, htonl(INADDR_ANY), htons(TCP_PORT));
    listen(sock, 5);

    for(;;) {
        Client *client = new Client(acceptConnection(listenSock));
    }
}
