/*------------------------------------------------------------------------------
-- SOURCE FILE: songqueue.cpp
--
-- PROGRAM: CommAudio
--
-- FUNCTIONS:
--          void addSong(Song song);
            void popSong();
            void sendSongQueue();
            void unlockedSendSongQueue();
--
-- DATE:    April 10th, 2017
--
-- DESIGNER: Brody McCrone
--
-- PROGRAMMER: Brody McCrone
--
-- NOTES: Functions for managing the song queue.
--
------------------------------------------------------------------------------*/
#include "songqueue.h"
#include <winsock2.h>
#include <windows.h>
#include <string.h>
#include "clientmanager.h"

SongQueue SongQueue::instance;

/*------------------------------------------------------------------------------
-- FUNCTION: void SongQueue::addSong(Song song)
--
-- DATE:    April 10th, 2017
--
-- DESIGNER: Brody McCrone
--
-- PROGRAMMER: Brody McCrone
--
-- INTERFACE: void SongQueue::addSong(Song song)
--
-- PARAMETERS: N/A
--
-- RETURNS: N/A
--
-- NOTES: Add song to the queue and send it to all connected clients.
--
------------------------------------------------------------------------------*/
void SongQueue::addSong(Song song) {
    std::lock_guard<std::mutex> guard(lock);
    queue.push_back(song);
    unlockedSendSongQueue();
}

/*------------------------------------------------------------------------------
-- FUNCTION: void SongQueue::popSong()
--
-- DATE:    April 10th, 2017
--
-- DESIGNER: Brody McCrone
--
-- PROGRAMMER: Brody McCrone
--
-- INTERFACE: void SongQueue::popSong()
--
-- PARAMETERS: N/A
--
-- RETURNS: N/A
--
-- NOTES: Take a song off the list and update all connected clients.
--
------------------------------------------------------------------------------*/
void SongQueue::popSong() {
    std::lock_guard<std::mutex> guard(lock);
    queue.erase(queue.begin());
    unlockedSendSongQueue();
}

/*------------------------------------------------------------------------------
-- FUNCTION: void SongQueue::sendSongQueue()
--
-- DATE:    April 10th, 2017
--
-- DESIGNER: Brody McCrone
--
-- PROGRAMMER: Brody McCrone
--
-- INTERFACE: void SongQueue::sendSongQueue()
--
-- PARAMETERS: N/A
--
-- RETURNS: N/A
--
-- NOTES: Put a mutex on the song queue and send the queue to all connected clients.
--
------------------------------------------------------------------------------*/
void SongQueue::sendSongQueue() {
    std::lock_guard<std::mutex> guard(lock);
    unlockedSendSongQueue();
}

/*------------------------------------------------------------------------------
-- FUNCTION: void SongQueue::unlockedSendSongQueue()
--
-- DATE:    April 10th, 2017
--
-- DESIGNER: Brody McCrone
--
-- PROGRAMMER: Brody McCrone
--
-- INTERFACE: void SongQueue::unlockedSendSongQueue()
--
-- PARAMETERS: N/A
--
-- RETURNS: N/A
--
-- NOTES: Send song queue too all connected clients through broadcast.
--
------------------------------------------------------------------------------*/
void SongQueue::unlockedSendSongQueue() {
    WSABUF wsaBuf;
    wsaBuf.buf = buffer;
    PktIds pktId = PktIds::SONG_QUEUE;
    int numSongs = static_cast<int>(queue.size());

    wsaBuf.len = sizeof(pktId) + sizeof(int) + (numSongs * sizeof(int));

    memcpy(wsaBuf.buf, &pktId, sizeof(pktId));
    int offset = sizeof(pktId);

    memcpy(wsaBuf.buf + offset, &numSongs, sizeof(int));
    offset += sizeof(int);

    for (auto& song : queue) {
        int songId = song.getId();
        memcpy(wsaBuf.buf + offset, &songId, sizeof(songId));
        offset += sizeof(songId);
    }

    ClientManager::get().broadcast(wsaBuf);
}
