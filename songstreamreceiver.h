#ifndef SONGSTREAMRECEIVER_H
#define SONGSTREAMRECEIVER_H

#include "routinestructs.h"
#include <winsock2.h>
#include <windows.h>
#include "packets.h"

class SongStreamReceiver {
public:
    SongStreamReceiver(int listenPort);
    SongStreamReceiver() = default;
    void init();
    static void CALLBACK receiveSongStreamRoutine(DWORD err, DWORD bytesRecv, LPWSAOVERLAPPED overlapped, DWORD flags);
    void handleSongPkt();
    OlapWrap getOlapWrap();
private:
    int sock;
    OlapWrap olapWrap;
    WSABUF wsaBuf;
    char buffer[sizeof(Audio)];
};

#endif // SONGSTREAMRECEIVER_H
