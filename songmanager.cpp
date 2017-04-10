#include "songmanager.h"
#include "packets.h"
#include <winsock2.h>
#include <windows.h>
#include "clientmanager.h"

SongManager SongManager::instance;

void SongManager::addSong(Song song) {
    std::lock_guard<std::mutex> guard(lock);
    map.insert(std::pair<int, Song>(song.getId(), song));

    PktIds pktId = PktIds::SONGS;
    int numSongs = static_cast<int>(map.size());

    std::vector<char> sendLine;
    WSABUF wsaBuf;
    wsaBuf.len = static_cast<u_long>(numSongs) * sizeof(SongInfo) + sizeof(PktIds::SONGS) + sizeof(int);
    sendLine.resize(wsaBuf.len);
    wsaBuf.buf = sendLine.data();

    memcpy(wsaBuf.buf, &pktId, sizeof(pktId));
    int offset = sizeof(pktId);

    memcpy(wsaBuf.buf, &numSongs, sizeof(numSongs));
    offset += sizeof(numSongs);

    for (auto& elem : map) {
        offset += elem.second.getInfoPktized(wsaBuf.buf + offset, false);
    }

    ClientManager::get().broadcast(wsaBuf);
}
