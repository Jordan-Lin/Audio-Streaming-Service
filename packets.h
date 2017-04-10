#ifndef PACKETS_H
#define PACKETS_H

#define AUDIO_BUFFER_SIZE 1024
#define SONG_TITLE_SIZE 256
#define SONG_ARTIST_SIZE 256
#define MAX_USERNAME 64

enum PktIds {
    ARTWORK,
    AUDIO,
    JOIN,
    SONG_INFO,
    USERS,
    USER_INFO
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
    char username[MAX_USERNAME];
} Join;

typedef struct {
    PktIds pktId = PktIds::SONG_INFO;
    int titleLen;
    char *title;
    int artistLen;
    char *artist;
    int albumLen;
    char *album;
    int artworkLen;
    char *artwork;
} SongInfo;

typedef struct {
    PktIds pktId = PktIds::USER_INFO;
    int userId;
    char username[MAX_USERNAME];
} UserInfo;

typedef struct {
    PktIds pktId = PktIds::USERS;
    int numUsers;
    UserInfo *users;
} Users;

#endif // PACKETS_H
