#ifndef SONGQUEUE_H
#define SONGQUEUE_H

#include "LockedQueue.h"
#include "Song.h"
#include <memory>
#include "defines.h"

class SongQueue : public LockedQueue<Song> {
public:
    static SongQueue& get() { return instance; }
    void addSong(Song song);
    void popSong();
    void sendSongQueue();
private:
    void unlockedSendSongQueue();

    static SongQueue instance;
    char buffer[LARGE_BUFFER_SIZE];
};

#endif
