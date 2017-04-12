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
    Song(int id, QString title, QString artist, QString album)
        : id(id), title(title), artist(artist), album(album) {
        hasArtwork = false;
    }
    
    Song(int id, QString title, QString artist, QString album, QString audioDir)
        : id(id), title(title), artist(artist), album(album), audioDir(audioDir) {
        hasArtwork = false;
    }

    void getInfoPktized(char *buf, bool withArtwork);
    std::vector<Audio> genAudioPkts();
    int getInfoPktLen(bool artwork);
    ~Song();
    int getId() const {return id;}
    QString getTitle() const {return title;}
    QString getArtist() const {return artist;}
    QString getAlbum() const {return album;}
    QString getDir() const {return audioDir;}

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
