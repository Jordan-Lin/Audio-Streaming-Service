#ifndef SONGSTREAMER_H
#define SONGSTREAMER_H

#include <winsock2.h>
#include <windows.h>
#include <vector>
#include "utilities.h"
#include "packets.h"
#include <string>
#include <QString>

class SongStreamer {
public:
    SongStreamer();
    ~SongStreamer();
    static void CALLBACK streamSongRoutine(DWORD err, DWORD bytesRecv, LPWSAOVERLAPPED overlapped, DWORD flags);

    void SongStreamer::initStream();
    void SongStreamer::streamSong();

private:
    void packetizeNextSongSection();

    HANDLE songSent;
    struct sockaddr_in addr;
    int totalBytes;
    int bytesSent;
    int sock;
    SongStreamerOlapWrap olapWrap;
    Audio audioPkt;
    QByteArray data;
    WSABUF wsaBuf;
};

#endif // SONGSTREAMER_H
