#include "songstreamer.h"
#include <string.h>
#include <QDebug>
#include "debugwindow.h"
#include <QFile>
#include <ws2tcpip.h>
#include "audiomanager.h"
#include "defines.h"
#include "songqueue.h"
#include "gettime.h"

SongStreamer::SongStreamer() {
    sock = createSocket(SOCK_DGRAM);
    bindSocket(sock, createAddress(htonl(INADDR_ANY), 0));
    addr = createAddress(inet_addr("234.5.6.8"), htons(6555));

    joinMulticast(sock);
    traverseMultiple(sock);
    disableLoopback(sock);
}

void SongStreamer::initStream() {
    songSent = CreateEvent(NULL, FALSE, FALSE, NULL);

    while (true) {
        if (SongQueue::get().size() == 0) {
            continue;
        }

        Song song = SongQueue::get().front();
        data = audioManager::get().loadAudio(song.getDir());

        HeaderInfo info = audioManager::get().parseHeader(audioManager::get().loadHeader(song.getDir()));
        WSABUF wsaBuf;
        wsaBuf.buf = reinterpret_cast<char *>(&info);
        wsaBuf.len = sizeof(HeaderInfo);
        sendUDP(sock, wsaBuf, addr);
        totalBytes = data.size();
        bytesSent = 0;
        Time begin = getCurrentTime();
        streamSong();
        DWORD waitResult = WAIT_IO_COMPLETION;
        while (waitResult == WAIT_IO_COMPLETION) {
            waitResult = WaitForSingleObjectEx(songSent, INFINITE, TRUE);
        }
        Time end = getCurrentTime();
        HANDLE tempEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
        DebugWindow::get()->logd(QString(" TIME : ")  + itoq(1000 * audioManager::get().durationCalc(
                audioManager::get().loadHeader(song.getDir()))));
        DebugWindow::get()->logd(QString("duration: ") + (1000 * audioManager::get().durationCalc(
                                     audioManager::get().loadHeader(song.getDir()))));
        Sleep(1000 * audioManager::get().durationCalc(
                audioManager::get().loadHeader(song.getDir())) - getDuration(begin, end)); //len of song
        SongQueue::get().popSong();
    }
}

void CALLBACK SongStreamer::streamSongRoutine(DWORD err, DWORD bytesRecv, LPWSAOVERLAPPED overlapped, DWORD flags) {
    if (err != 0) {
    } else {
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
    wsaBuf.len = sizeof(Audio);
    wsaBuf.buf = reinterpret_cast<char *>(&audioPkt);
}
