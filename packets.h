#ifndef PACKETS_H
#define PACKETS_H

#define AUDIO_BUFFER_SIZE 1024
#define SONG_TITLE_SIZE 256
#define SONG_ARTIST_SIZE 256
#define MAX_USERNAME 64
#define MAX_TITLE 64
#define MAX_ARTIST 64
#define MAX_ALBUM 64

enum PktIds {
    ARTWORK,
    AUDIO,
    JOIN,
    SONG_INFO,
    USERS,
    USER_INFO,
    SONGS,
    SONG_REQUEST,
    SONG_QUEUE
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
    int songId;
    char title[MAX_TITLE];
    char artist[MAX_ARTIST];
    char album[MAX_ALBUM];
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

typedef struct {
    PktIds pktId = PktIds::SONGS;
    int numSongs;
    SongInfo *songs;
} Songs;

typedef struct {
    PktIds pktId = PktIds::SONG_QUEUE;
    int numSongs;
    int *songIds;
} SongsQueued;

typedef struct {
    PktIds pktId = PktIds::SONG_REQUEST;
    int songId;
} SongRequest;

#endif // PACKETS_H
