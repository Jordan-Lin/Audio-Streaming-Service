#include "songstreamreceiver.h"
#include "utilities.h"
#include "debugwindow.h"
#include <QDebug>
#include "audiomanager.h"

SongStreamReceiver::SongStreamReceiver(int listenPort) {
    sock = createSocket(SOCK_DGRAM);
    bindSocket(sock, createAddress(htonl(INADDR_ANY), htons(listenPort)));
    wsaBuf.buf = buffer;
    wsaBuf.len = sizeof(Audio);
    gotPacket = CreateEvent(NULL, FALSE, FALSE, NULL);
}

void SongStreamReceiver::init() {
    //joinMulticast(sock);
    olapWrap.receiver = this;
    receive(sock, wsaBuf, &olapWrap.olap, receiveSongStreamRoutine);
    while(true) {
        WaitForSingleObjectEx(gotPacket, INFINITE, TRUE);
    }
}

void CALLBACK SongStreamReceiver::receiveSongStreamRoutine(DWORD err, DWORD bytesRecv, LPWSAOVERLAPPED overlapped, DWORD flags) {
    if (err != 0) {
        DebugWindow::get()->logd(QString("receiveSongStreamRoutine failed, error code: ") + itoq(err));
    } else {
        DebugWindow::get()->logd("receiveSongStreamRoutine succeeded");
    }

    reinterpret_cast<SongStreamReceiverOlapWrap *>(overlapped)->receiver->handleSongPkt(bytesRecv);
}

void SongStreamReceiver::handleSongPkt(DWORD recvBytes) {
    char *tempBuffer = buffer;
    int offset = 0;
    while (recvBytes) {
        tempBuffer += offset;
        offset = 0;
        int *id = reinterpret_cast<int *>(tempBuffer);
        switch (*id) {
        case AUDIO:
            {
                Audio *audio = reinterpret_cast<Audio *>(tempBuffer);
                audioManager::get().appender(QByteArray(audio->buffer, audio->len));
                audioManager::get().playSong();
            }
            offset += sizeof(Audio);
            break;
        case AUDIO_FORMAT_ID:
            {
                HeaderInfo *info = reinterpret_cast<HeaderInfo *>(tempBuffer);
                std::thread audioThread(&audioManager::initAudio,
                        &audioManager::get(), info->bitsPerSample,
                        info->sampleRate, info->numberOfChannels);
                audioThread.detach();
            }
            offset += sizeof(HeaderInfo);
            break;
        }
        recvBytes -= offset;
    }
    SetEvent(gotPacket);
    wsaBuf.buf = buffer;
    wsaBuf.len = sizeof(Audio);
    olapWrap.receiver = this;
    receive(sock, wsaBuf, &olapWrap.olap, receiveSongStreamRoutine);
}
