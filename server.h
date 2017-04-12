#ifndef SERVER_H
#define SERVER_H

#include <winsock2.h>
#include <windows.h>
#include "defines.h"
#include <atomic>
#include "songstreamer.h"

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
    SongStreamer *streamer;

    SOCKET listenSock;
    std::atomic<ServerState> state{ServerState::NOT_RUNNING};

    void run();
    void addDummySongs();
};

#endif // SERVER_H
