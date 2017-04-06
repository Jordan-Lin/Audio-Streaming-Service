#include "songstreamreceiver.h"
#include "utilities.h"
#include "MainWindow.h"
#include <QDebug>

SongStreamReceiver::SongStreamReceiver(int listenPort) {
    sock = createSocket(SOCK_DGRAM);
    bindSocket(sock, createAddress(htonl(INADDR_ANY), htons(listenPort)));
    wsaBuf.buf = buffer;
    wsaBuf.len = sizeof(Audio);
    gotPacket = CreateEvent(NULL, FALSE, FALSE, NULL);
}

void SongStreamReceiver::init() {
    olapWrap.receiver = this;
    receive(sock, wsaBuf, &olapWrap.olap, receiveSongStreamRoutine);
    while(true) {
        WaitForSingleObjectEx(gotPacket, INFINITE, TRUE);
    }
}

void CALLBACK SongStreamReceiver::receiveSongStreamRoutine(DWORD err, DWORD bytesRecv, LPWSAOVERLAPPED overlapped, DWORD flags) {
    if (err != 0) {
        MainWindow::get()->logd(QString("receiveSongStreamRoutine failed, error code: ") + itoq(err));
    } else {
        MainWindow::get()->logd("receiveSongStreamRoutine succeeded");
    }

    reinterpret_cast<SongStreamReceiverOlapWrap *>(overlapped)
        ->receiver->handleSongPkt();
}

void SongStreamReceiver::handleSongPkt() {
    SetEvent(gotPacket);
    Audio *audio = reinterpret_cast<Audio *>(buffer);
    MainWindow::get()->logpi(addNull(audio->buffer, audio->len));
    wsaBuf.buf = buffer;
    wsaBuf.len = sizeof(Audio);
    olapWrap.receiver = this;
    receive(sock, wsaBuf, &olapWrap.olap, receiveSongStreamRoutine);
}
