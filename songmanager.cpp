#include "songmanager.h"
#include "packets.h"
#include <QDir>
#include <winsock2.h>
#include <windows.h>
#include "clientmanager.h"
#include "debugwindow.h"

SongManager SongManager::instance;

void SongManager::addSong(Song song) {
    std::lock_guard<std::mutex> guard(lock);
    map.insert(std::pair<int, Song>(song.getId(), song));
    unlockedSendSongList();
}

void SongManager::sendSongList() {
    std::lock_guard<std::mutex> guard(lock);
    unlockedSendSongList();
}

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

void SongManager::LoadSongList()
{
    QString testAudioFilePath = "C:\\Users\\Brody McCrone\\Desktop\\CommAudioWavFiles";                    // Test, change to directory next to app

    DebugWindow::get()->logd(testAudioFilePath);
    QDir testAudioDirectory;                                                                    // Test, change to directory next to app
    testAudioDirectory.setPath(testAudioFilePath);
    DebugWindow::get()->logd("Setting list filters");
    QStringList filters;
    filters << "*.wav";
    testAudioDirectory.setNameFilters(filters);
    testAudioDirectory.setSorting(QDir::Name);

    DebugWindow::get()->logd("Getting entryList");
    QStringList files = testAudioDirectory.entryList();
    for (const QString& f : files) {
        Song temp(SongManager::get().genId(), f, "Unknown", "Uknown", testAudioFilePath + "\\" + f);
        SongManager::get().addSong(temp);
    }
}
