#ifndef SONG_H
#define SONG_H

#include <QString>
#include <packets.h>
#include <vector>

class Song {
public:
    Song(const char *songInfoPkt);
    Song(QString title, QString artist, QString album, QString audioDir)
        : title(title), artist(artist), album(album), audioDir(audioDir) {
        hasArtwork = false;
    }
    Song(QString title, QString artist, QString album, QString audioDir, QString artworkDir)
        : title(title), artist(artist), album(album), audioDir(audioDir), artworkDir(artworkDir) {
        hasArtwork = true;
    }
    void genSongInfoPkt(char *buf);
    std::vector<Audio> genAudioPkts();
private:
    QString title;
    QString artist;
    QString album;
    QString audioDir;
    bool hasArtwork;
    QString artworkDir;
};

#endif // SONG_H
