#include "songstreamer.h"
#include <string.h>
#include <QDebug>
#include "mainwindow.h"
#include <QFile>
#include <ws2tcpip.h>

SongStreamer::SongStreamer() {
    sock = createSocket(SOCK_DGRAM);
    bindSocket(sock, createAddress(htonl(INADDR_ANY), 0));
    addr = createAddress(inet_addr("127.0.0.1"), htons(5150));

    joinMulticast(sock);
    traverseMultiple(sock);
    disableLoopback(sock);
}

void SongStreamer::initStream(QString fileName) {
    songSent = CreateEvent(NULL, FALSE, FALSE, NULL);

    QFile file(fileName);
    file.open(QIODevice::ReadOnly);
    if (file.isOpen()) {
        MainWindow::get()->logd("FILE IS OPEN");
    } else {
        MainWindow::get()->logd(QString("FILE IS NOT OPEN"));
    }
    data = file.readAll();
    totalBytes = data.size();
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
    memcpy(audioPkt.buffer, data.data() + bytesSent, audioPkt.len);
    MainWindow::get()->logpo(addNull(data.data() + bytesSent, audioPkt.len));
    //MainWindow::get()->logpo(QString("PacketStart:") + addNull(audioPkt.buffer, audioPkt.len) + ":PacketEnd");
    wsaBuf.len = sizeof(Audio);
    MainWindow::get()->logd(QString("wsaBuf.len = ") + itoq(wsaBuf.len) + ", audioPkt.len = " + itoq(audioPkt.len));
    wsaBuf.buf = reinterpret_cast<char *>(&audioPkt);
}
