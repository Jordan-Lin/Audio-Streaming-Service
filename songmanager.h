#ifndef SONGMANAGER_H
#define SONGMANAGER_H

#include "LockedMap.h"
#include "Song.h"
#include <memory>

class SongManager : public LockedMap<int, Song> {
public:
    static SongManager& get() {return instance;}
    int genId() {return ++idCounter;}
    void addSong(Song song);
private:
    int idCounter = 0;
    static SongManager instance;
};

#endif
