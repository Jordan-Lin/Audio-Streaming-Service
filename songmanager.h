#ifndef SONGMANAGER_H
#define SONGMANAGER_H

#include "LockedMap.h"
#include "SongManager.h"
#include "Song.h"

class SongManager : LockedMap<int, Song> {
public:
    static SongManager& get() {return instance;};
private:
    static SongManager& instance;
};

#endif
