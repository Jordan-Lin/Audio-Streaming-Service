#include "songstreamer.h"
#include <string.h>
#include <QDebug>

SongStreamer SongStreamer::instance;

SongStreamer::SongStreamer() {
    startup();
    sock = createSocket(SOCK_DGRAM);
    addr = createAddress(inet_addr("127.0.0.1"), htons(5150));
    memset(&olap, 0, sizeof(OVERLAPPED));
}

void SongStreamer::initStream(const char *fileName) {
    std::ifstream file(fileName, std::ios::binary);
    totalBytes = 0;
    buffer.resize(0);
    while (true) {
        buffer.resize(buffer.size() + AUDIO_BUFFER_SIZE);
        if (!file.read(buffer.data() + totalBytes, AUDIO_BUFFER_SIZE)) {
            totalBytes += file.gcount();
            break;
        } else {
            totalBytes += AUDIO_BUFFER_SIZE;
        }
    }
    bytesSent = 0;
    streamSong();
}

void CALLBACK SongStreamer::streamSongRoutine(DWORD err, DWORD bytesRecv, LPWSAOVERLAPPED overlapped, DWORD flags) {
    if (err != 0) {
        qDebug() << "streamRecvRoutine failed, error code: " << itoq(err);
    }

    SongStreamer::get().streamSong();
}

void SongStreamer::streamSong() {
    qDebug() << "bytesSent = " << bytesSent << ", totalBytes: " << totalBytes;
    if (bytesSent < totalBytes) {
        packetizeNextSongSection();
        multicast(sock, buf, addr, olap, streamSongRoutine);
    }
}

void SongStreamer::packetizeNextSongSection() {
    int len;
    if (totalBytes - bytesSent < AUDIO_BUFFER_SIZE) {
        len = totalBytes - bytesSent;
    } else {
        len = AUDIO_BUFFER_SIZE;
    }
    buf.buf = (char *)malloc(len);
    buf.len = len;
    bytesSent += len;
}
