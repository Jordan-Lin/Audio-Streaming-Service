#include "songstreamreceiver.h"
#include "utilities.h"
#include <QDebug>

SongStreamReceiver::SongStreamReceiver(int listenPort) {
    sock = createSocket(SOCK_DGRAM);
    bindSocket(sock, createAddress(INADDR_ANY, htons(listenPort)));
    wsaBuf.buf = buffer;
    wsaBuf.len = sizeof(Audio);
    olapWrap.receiver = this;
}

void SongStreamReceiver::init() {
    receive(sock, wsaBuf, olapWrap.olap, receiveSongStreamRoutine);
}

void CALLBACK SongStreamReceiver::receiveSongStreamRoutine(DWORD err, DWORD bytesRecv, LPWSAOVERLAPPED overlapped, DWORD flags) {
    if (err != 0) {
        qDebug() << "receiveSongStreamRoutine failed, error code: " << itoq(err);
    } else {
        qDebug() << "receiveSongStreamRoutine succeeded";
    }

    OlapWrap *o = reinterpret_cast<OlapWrap *>(overlapped);
    o->receiver->handleSongPkt();
}

void SongStreamReceiver::handleSongPkt() {
    wsaBuf.buf = buffer;
    wsaBuf.len = sizeof(Audio);
    Audio *audio = reinterpret_cast<Audio *>(buffer);
    qDebug() << audio->buffer;
    receive(sock, wsaBuf, olapWrap.olap, receiveSongStreamRoutine);
}
