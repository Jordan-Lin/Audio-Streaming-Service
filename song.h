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
    int getInfoPktized(char *buf, bool withArtwork);
    std::vector<Audio> genAudioPkts();

    int getId() const {return id;}
private:
    int id;
    QString title;
    QString artist;
    QString album;
    QString audioDir;
    bool hasArtwork;
    QString artworkDir;
};

#endif // SONG_H
