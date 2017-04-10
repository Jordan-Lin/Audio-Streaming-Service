#include "songqueue.h"
#include <winsock2.h>
#include <windows.h>
#include <string.h>
#include "clientmanager.h"

SongQueue SongQueue::instance;

void SongQueue::addSong(Song song) {
    std::lock_guard<std::mutex> guard(lock);
    queue.push_back(song);
    sendSongQueue();
}

void SongQueue::popSong() {
    std::lock_guard<std::mutex> guard(lock);
    queue.erase(queue.begin());
    sendSongQueue();
}

void SongQueue::sendSongQueue() {
    PktIds pktId = PktIds::SONG_QUEUE;
    int numSongs = static_cast<int>(queue.size());

    std::vector<char> sendLine;
    WSABUF wsaBuf;
    wsaBuf.len = sizeof(pktId) + sizeof(int) + numSongs;
    sendLine.resize(wsaBuf.len);
    wsaBuf.buf = sendLine.data();

    memcpy(wsaBuf.buf, &pktId, sizeof(pktId));
    int offset = sizeof(pktId);

    memcpy(wsaBuf.buf, &numSongs, sizeof(int));
    offset += sizeof(int);

    for (auto& song : queue) {
        int songId = song.getId();
        memcpy(wsaBuf.buf, &songId, sizeof(songId));
        offset += sizeof(songId);
    }

    ClientManager::get().broadcast(wsaBuf);
}
