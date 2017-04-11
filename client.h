#ifndef CLIENT_H
#define CLIENT_H

#include <winsock2.h>
#include <windows.h>
#include "routinestructs.h"
#include "defines.h"
#include <QString>
#include "song.h"

class Client {
public:
    Client(QString serverIp, QString username);
    static void receiveRoutine(DWORD errCode, DWORD recvBytes, LPOVERLAPPED olap, DWORD flags);
    void parse(int recvBytes);
    void sendSongRequest(int songId);
private:
    SOCKET sock;
    WSABUF wsaBuf;
    ClientOlap olapWrap;
    char buffer[LARGE_BUFFER_SIZE];
    struct sockaddr_in servAddr;
    int id;

    void run(QString username);
};

#endif // CLIENT_H
