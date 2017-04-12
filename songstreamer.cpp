/*------------------------------------------------------------------------------
-- SOURCE FILE: songstreamer.cpp
--
-- PROGRAM: CommAudio
--
-- FUNCTIONS:
--          SongStreamer();
            ~SongStreamer();
            static void CALLBACK streamSongRoutine(DWORD err, DWORD bytesRecv, LPWSAOVERLAPPED overlapped, DWORD flags);
            void SongStreamer::initStream();
            void SongStreamer::streamSong();
            void packetizeNextSongSection();
--
-- DATE:    April 10th, 2017
--
-- DESIGNER: Brody McCrone
--
-- PROGRAMMER: Brody McCrone
--
-- NOTES: Functions to stream the songs.
--
------------------------------------------------------------------------------*/
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

/*------------------------------------------------------------------------------
-- FUNCTION: SongStreamer::SongStreamer()
--
-- DATE:    April 10th, 2017
--
-- DESIGNER: Brody McCrone
--
-- PROGRAMMER: Brody McCrone
--
-- INTERFACE: SongStreamer::SongStreamer()
--
-- PARAMETERS: N/A
--
-- RETURNS: N/A
--
-- NOTES: Constructor for SongStreamer, join to a multicast network.
--
------------------------------------------------------------------------------*/
SongStreamer::SongStreamer() {
    sock = createSocket(SOCK_DGRAM);
    bindSocket(sock, createAddress(htonl(INADDR_ANY), 0));
    addr = createAddress(inet_addr("234.5.6.8"), htons(6555));

    joinMulticast(sock);
    traverseMultiple(sock);
    disableLoopback(sock);
}

/*------------------------------------------------------------------------------
-- FUNCTION: void SongStreamer::initStream()
--
-- DATE:    April 10th, 2017
--
-- DESIGNER: Brody McCrone
--
-- PROGRAMMER: Brody McCrone
--
-- INTERFACE: void SongStreamer::initStream()
--
-- PARAMETERS: N/A
--
-- RETURNS: N/A
--
-- NOTES: Starts the streaming of songs; time the operation.
--
------------------------------------------------------------------------------*/
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

/*------------------------------------------------------------------------------
-- FUNCTION: void CALLBACK SongStreamer::streamSongRoutine(DWORD err, DWORD bytesRecv, LPWSAOVERLAPPED overlapped, DWORD flags)
--
-- DATE:    April 10th, 2017
--
-- DESIGNER: Brody McCrone
--
-- PROGRAMMER: Brody McCrone
--
-- INTERFACE: void CALLBACK SongStreamer::streamSongRoutine(DWORD err, DWORD bytesRecv, LPWSAOVERLAPPED overlapped, DWORD flags)
--
-- PARAMETERS: N/A
--
-- RETURNS: N/A
--
-- NOTES: The routine function for streaming songs.
--
------------------------------------------------------------------------------*/
void CALLBACK SongStreamer::streamSongRoutine(DWORD err, DWORD bytesRecv, LPWSAOVERLAPPED overlapped, DWORD flags) {
    if (err != 0) {
    } else {
    }

    reinterpret_cast<SongStreamerOlapWrap *>(overlapped)->sender->streamSong();
}

/*------------------------------------------------------------------------------
-- FUNCTION: void SongStreamer::streamSong()
--
-- DATE:    April 10th, 2017
--
-- DESIGNER: Brody McCrone
--
-- PROGRAMMER: Brody McCrone
--
-- INTERFACE: void SongStreamer::streamSong()
--
-- PARAMETERS: N/A
--
-- RETURNS: N/A
--
-- NOTES: Stream the section of the song that has been packetized.
--
------------------------------------------------------------------------------*/
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

/*------------------------------------------------------------------------------
-- FUNCTION: void SongStreamer::packetizeNextSongSection()
--
-- DATE:    April 10th, 2017
--
-- DESIGNER: Brody McCrone
--
-- PROGRAMMER: Brody McCrone
--
-- INTERFACE: void SongStreamer::packetizeNextSongSection()
--
-- PARAMETERS: N/A
--
-- RETURNS: N/A
--
-- NOTES: Packetize the next section in the song to prepare it to be sent.
--
------------------------------------------------------------------------------*/
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
