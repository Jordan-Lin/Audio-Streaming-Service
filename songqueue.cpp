#include "songqueue.h"
#include <winsock2.h>
#include <windows.h>
#include <string.h>
#include "clientmanager.h"

SongQueue SongQueue::instance;

void SongQueue::addSong(Song song) {
    std::lock_guard<std::mutex> guard(lock);
    queue.push_back(song);
    unlockedSendSongQueue();
}

void SongQueue::popSong() {
    std::lock_guard<std::mutex> guard(lock);
    queue.erase(queue.begin());
    unlockedSendSongQueue();
}

void SongQueue::sendSongQueue() {
    std::lock_guard<std::mutex> guard(lock);
    unlockedSendSongQueue();
}

void SongQueue::unlockedSendSongQueue() {
    WSABUF wsaBuf;
    wsaBuf.buf = buffer;
    PktIds pktId = PktIds::SONG_QUEUE;
    int numSongs = static_cast<int>(queue.size());

    wsaBuf.len = sizeof(pktId) + sizeof(int) + (numSongs * sizeof(int));

    memcpy(wsaBuf.buf, &pktId, sizeof(pktId));
    int offset = sizeof(pktId);

    memcpy(wsaBuf.buf, &numSongs, sizeof(int));
    offset += sizeof(int);

    for (auto& song : queue) {
        int songId = song.getId();
        memcpy(wsaBuf.buf + offset, &songId, sizeof(songId));
        offset += sizeof(songId);
    }

    ClientManager::get().broadcast(wsaBuf);
}
