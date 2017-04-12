/*------------------------------------------------------------------------------
-- SOURCE FILE: songmanager.cpp
--
-- PROGRAM: CommAudio
--
-- FUNCTIONS:
-- void SongManager::addSong(Song song)
-- void SongManager::sendSongList()
-- void SongManager::unlockedSendSongList()
-- void SongManager::LoadSongList()
--
-- DATE:    April 11th, 2017
--
-- DESIGNER: Justen DePourcq, Trista Huang, Jordan Lin, Brody McCrone
--
-- PROGRAMMER: Brody McCrone
--
-- NOTES: file that updates and sends the songlist to the clients
--
------------------------------------------------------------------------------*/
#include "songmanager.h"
#include "packets.h"
#include <QDir>
#include <winsock2.h>
#include <windows.h>
#include "clientmanager.h"
#include "debugwindow.h"

SongManager SongManager::instance;

/*------------------------------------------------------------------------------
-- FUNCTION: addSong
--
-- DATE:    April 11th, 2017
--
-- DESIGNER: Justen DePourcq, Trista Huang, Jordan Lin, Brody McCrone
--
-- PROGRAMMER: Justen DePourcq
--
-- INTERFACE: void SongManager::addSong(Song song)
--
-- PARAMETERS: Song song - struct containing the song information
--
-- RETURNS: N/A
--
-- NOTES: This function adds a song to the list of available songs on the server
--
------------------------------------------------------------------------------*/
void SongManager::addSong(Song song) {
    std::lock_guard<std::mutex> guard(lock);
    map.insert(std::pair<int, Song>(song.getId(), song));
    unlockedSendSongList();
}

/*------------------------------------------------------------------------------
-- FUNCTION: sendSongList
--
-- DATE:    April 11th, 2017
--
-- DESIGNER: Justen DePourcq, Trista Huang, Jordan Lin, Brody McCrone
--
-- PROGRAMMER: Brody McCrone
--
-- INTERFACE: void SongManager::sendSongList()
--
-- PARAMETERS: N/A
--
-- RETURNS: N/A
--
-- NOTES: This function calls the function that sends the song list which is
--        locked by a mutex
--
------------------------------------------------------------------------------*/
void SongManager::sendSongList() {
    std::lock_guard<std::mutex> guard(lock);
    unlockedSendSongList();
}

/*------------------------------------------------------------------------------
-- FUNCTION: unlockedSendSongList
--
-- DATE:    April 11th, 2017
--
-- DESIGNER: Justen DePourcq, Trista Huang, Jordan Lin, Brody McCrone
--
-- PROGRAMMER: Brody McCrone
--
-- INTERFACE: void SongManager::unlockedSendSongList()
--
-- PARAMETERS: N/A
--
-- RETURNS: N/A
--
-- NOTES: Sends the song list to the client.
--
------------------------------------------------------------------------------*/
void SongManager::unlockedSendSongList() {
    WSABUF wsaBuf;
    wsaBuf.buf = buffer;

    int numSongs = static_cast<int>(map.size());
    PktIds pktId = PktIds::SONGS;

    memcpy(wsaBuf.buf, &pktId, sizeof(PktIds::SONGS));
    int offset = sizeof(PktIds::SONGS);

    memcpy(wsaBuf.buf + offset, &numSongs, sizeof(numSongs));
    offset += sizeof(int);

    for (auto& elem : map) {
        const int pktLen = elem.second.getInfoPktLen(false);
        elem.second.getInfoPktized(wsaBuf.buf + offset, false);
        offset += pktLen;
    }

    wsaBuf.len = offset;

    ClientManager::get().broadcast(wsaBuf);
}

/*------------------------------------------------------------------------------
-- FUNCTION: LoadSongList
--
-- DATE:    April 11th, 2017
--
-- DESIGNER: Justen DePourcq, Trista Huang, Jordan Lin, Brody McCrone
--
-- PROGRAMMER: Jordan Lin
--
-- INTERFACE: void SongManager::LoadSongList()
--
-- PARAMETERS: N/A
--
-- RETURNS: N/A
--
-- NOTES: Loads the list of songs stored in a file on the server
--
------------------------------------------------------------------------------*/
void SongManager::LoadSongList()
{
    QString songdirectory = QDir::currentPath() + QString("/Songs");
    DebugWindow::get()->logd(songdirectory);
    QDir audioDirectory;
    audioDirectory.setPath(songdirectory);
    DebugWindow::get()->logd("Setting list filters");
    QStringList filters;
    filters << "*.wav";
    audioDirectory.setNameFilters(filters);
    audioDirectory.setSorting(QDir::Name);

    DebugWindow::get()->logd("Getting entryList");
    QStringList files = audioDirectory.entryList();
    for (const QString& f : files) {
        Song temp(SongManager::get().genId(), f, "Unknown", "Uknown", songdirectory + "\\" + f);
        SongManager::get().addSong(temp);
    }
}
