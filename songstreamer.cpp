#include "songstreamer.h"
#include <string.h>
#include <QDebug>
#include "mainwindow.h"

SongStreamer::SongStreamer() {
    sock = createSocket(SOCK_DGRAM);
    addr = createAddress(inet_addr("127.0.0.1"), htons(5150));
}

void SongStreamer::initStream(std::string fileName) {
    songSent = CreateEvent(NULL, FALSE, FALSE, NULL);
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
    MainWindow::get()->logd(QString("total bytes: ") + itoq(totalBytes));
    bytesSent = 0;
    streamSong();
    WaitForSingleObjectEx(songSent, INFINITE, TRUE);
}

void CALLBACK SongStreamer::streamSongRoutine(DWORD err, DWORD bytesRecv, LPWSAOVERLAPPED overlapped, DWORD flags) {
    if (err != 0) {
        MainWindow::get()->logd(QString("streamSongRoutine send failed, error code: ") + itoq(err));
    } else {
        MainWindow::get()->logd("streamSongRoutine send succeeded");
    }

    reinterpret_cast<SongStreamerOlapWrap *>(overlapped)->sender->streamSong();
}

void SongStreamer::streamSong() {
    if (bytesSent < totalBytes) {
        packetizeNextSongSection();
        olapWrap.sender = this;
        multicast(sock, &wsaBuf, addr, &olapWrap.olap, streamSongRoutine);
        bytesSent += audioPkt.len;
    } else {
        SetEvent(songSent);
    }
}

void SongStreamer::packetizeNextSongSection() {
    if (totalBytes - bytesSent < AUDIO_BUFFER_SIZE) {
        audioPkt.len = totalBytes - bytesSent;
    } else {
        audioPkt.len = AUDIO_BUFFER_SIZE;
    }
    memcpy(audioPkt.buffer, buffer.data() + bytesSent, audioPkt.len);
    MainWindow::get()->logpo(addNull(buffer.data() + bytesSent, audioPkt.len));
    //MainWindow::get()->logpo(QString("PacketStart:") + addNull(audioPkt.buffer, audioPkt.len) + ":PacketEnd");
    wsaBuf.len = sizeof(Audio);
    MainWindow::get()->logd(QString("wsaBuf.len = ") + itoq(wsaBuf.len) + ", audioPkt.len = " + itoq(audioPkt.len));
    wsaBuf.buf = reinterpret_cast<char *>(&audioPkt);
}
