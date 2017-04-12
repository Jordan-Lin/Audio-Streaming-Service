#ifndef SONGSTREAMRECEIVER_H
#define SONGSTREAMRECEIVER_H

#include "routinestructs.h"
#include <winsock2.h>
#include <windows.h>
#include "packets.h"

class SongStreamReceiver {
public:
    SongStreamReceiver(int listenPort);
    void init();
    static void CALLBACK receiveSongStreamRoutine(DWORD err, DWORD bytesRecv, LPWSAOVERLAPPED overlapped, DWORD flags);
    void handleSongPkt(DWORD recvBytes);
private:
    HANDLE gotPacket;
    int sock;
    SongStreamReceiverOlapWrap olapWrap;
    WSABUF wsaBuf;
    char buffer[sizeof(Audio)];
};

#endif // SONGSTREAMRECEIVER_H
