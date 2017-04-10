#ifndef SONGQUEUE_H
#define SONGQUEUE_H

#include "LockedQueue.h"
#include "Song.h"
#include <memory>

class SongQueue : public LockedQueue<Song> {
public:
    static SongQueue& get() { return instance; }
    void addSong(Song song);
    void popSong();

private:
    void sendSongQueue();
    static SongQueue instance;
};

#endif
