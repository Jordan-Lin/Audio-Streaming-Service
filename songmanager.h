#ifndef SONGMANAGER_H
#define SONGMANAGER_H

#include "LockedMap.h"
#include "Song.h"
#include <memory>
#include <winsock2.h>
#include <windows.h>
#include "defines.h"

class SongManager : public LockedMap<int, Song> {
public:
    static SongManager& get() {return instance;}
    int genId() {return ++idCounter;}
    void addSong(Song song);
    void sendSongList();
private:
    char buffer[LARGE_BUFFER_SIZE];
    int idCounter = 0;
    static SongManager instance;

    void unlockedSendSongList();
};

#endif
