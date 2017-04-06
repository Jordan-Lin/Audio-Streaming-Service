#ifndef PACKETS_H
#define PACKETS_H

#define AUDIO_BUFFER_SIZE 1024
#define USERNAME_SIZE 32
#define SONG_TITLE_SIZE 256
#define SONG_ARTIST_SIZE 256

enum PktIds {
    ARTWORK,
    AUDIO,
    JOIN,
    SONG_INFO

};

typedef struct {
    PktIds pktId = PktIds::ARTWORK;
    int len;
    char *buffer;
} Artwork;

typedef struct {
    PktIds pktId = PktIds::AUDIO;
    int len;
    char buffer[AUDIO_BUFFER_SIZE];
} Audio;

typedef struct {
    PktIds pktId = PktIds::JOIN;
    char username[USERNAME_SIZE];
} Join;

typedef struct {
    PktIds pktId = PktIds::SONG_INFO;
    char title[SONG_TITLE_SIZE];
    char artist[SONG_ARTIST_SIZE];
} SongInfo;


#endif // PACKETS_H
