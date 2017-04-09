#ifndef CLIENT_H
#define CLIENT_H

#include "winsock2.h"
#include "windows.h"
#include "packets.h"

#define BUFFER_SIZE 8192

class Client {
public:
    Client(SOCKET sock);
    UserInfo& getInfo() {return info;}
    SOCKET getSocket() {return info.userId;}
private:
    UserInfo info;
    std::string buffer;
    ClientOlap olapWrap;
    WSABuf buf;
    char buffer[BUFFER_SIZE];

    void init();
    static void CALLBACK receiveRoutine(DWORD errCode, DWORD recvBytes, LPOVERLAPPED olap, DWORD flags);
    void parse(int recvBytes);
};

#endif
