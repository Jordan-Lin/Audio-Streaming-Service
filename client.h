#ifndef CLIENT_H
#define CLIENT_H

#include <winsock2.h>
#include <windows.h>
#include "routinestructs.h"
#include "defines.h"
#include <QString>

class Client {
public:
    Client(QString serverIp, QString username);
    static void receiveRoutine(DWORD errCode, DWORD recvBytes, LPOVERLAPPED olap, DWORD flags);
    void parse(int recvBytes);
private:
    SOCKET sock;
    WSABUF wsaBuf;
    ClientOlap olapWrap;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in servAddr;

    void run(QString username);
};

#endif // CLIENT_H
