#include "server.h"
#include "clienthandler.h"
#include "clientmanager.h"
#include <thread>
#include "utilities.h"

Server::Server() {
    std::thread serverThread(&Server::run, this);
    serverThread.detach();
}

void Server::run() {
    listenSock = createSocket(SOCK_STREAM);
    bindSocket(listenSock, createAddress(htonl(INADDR_ANY), htons(REQUEST_PORT)));
    listen(listenSock, 5);

    for(;;) {
        ClientHandler *client = new ClientHandler(acceptConnection(listenSock));
    }
}
