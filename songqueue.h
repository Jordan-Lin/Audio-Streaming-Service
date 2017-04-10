#ifndef SONGQUEUE_H
#define SONGQUEUE_H

#include "LockedQueue.h"
#include "Song.h"

class SongQueue : LockedQueue<Song> {
public:
    static SongQueue& get() { return instance; }
private:
    static SongQueue& instance;
};

#endif
