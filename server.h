#ifndef SERVER_H
#define SERVER_H

#include <winsock2.h>
#include <windows.h>
#include "defines.h"
#include <atomic>

enum ServerState {
    NOT_RUNNING,
    INITIALIZING,
    RUNNING
};

class Server {
public:
    Server();
    ServerState getState() {return state;}
private:
    SOCKET listenSock;
    std::atomic<ServerState> state{ServerState::NOT_RUNNING};

    void run();
    void addDummySongs();
};

#endif // SERVER_H
