#ifndef CLIENT_H
#define CLIENT_H

#include "winsock2.h"
#include "windows.h"
#include "packets.h"
#include "routinestructs.h"
#include <string>
#include "defines.h"

class ClientHandler {
public:
    ClientHandler(SOCKET sock);
    UserInfo& getInfo() {return info;}
    SOCKET getSocket() {return info.userId;}
private:
    UserInfo info;
    ClientOlap olapWrap;
    WSABUF wsaBuf;
    char buffer[BUFFER_SIZE];

    void init();
    static void CALLBACK receiveRoutine(DWORD errCode, DWORD recvBytes, LPOVERLAPPED olap, DWORD flags);
    void parse(int recvBytes);
};

#endif
