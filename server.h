#ifndef SERVER_H
#define SERVER_H

#include <winsock2.h>
#include <windows.h>
#include "defines.h"

class Server {
public:
    Server();
private:
    SOCKET listenSock;

    void run();
};

#endif // SERVER_H
