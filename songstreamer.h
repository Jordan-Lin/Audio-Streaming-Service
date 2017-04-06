#ifndef SONGSTREAMER_H
#define SONGSTREAMER_H

#include <vector>
#include "utilities.h"
#include "packets.h"

class SongStreamer {
public:
    static SongStreamer& get() {return instance;}
    static void CALLBACK streamSongRoutine(DWORD err, DWORD bytesRecv, LPWSAOVERLAPPED overlapped, DWORD flags);

    void SongStreamer::initStream(const char *fileName);
    void SongStreamer::streamSong();
private:
    SongStreamer();

    static SongStreamer instance;
    void packetizeNextSongSection();

    int sock;
    struct sockaddr_in addr;
    OVERLAPPED olap;
    WSABUF buf;
    std::vector<char> buffer;
    Audio audioPkt;
    int totalBytes;
    int bytesSent;
};

#endif // SONGSTREAMER_H
