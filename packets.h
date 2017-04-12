#ifndef PACKETS_H
#define PACKETS_H

#include <QtGlobal>
#include <winsock2.h>
#include <windows.h>
#include "defines.h"

#define AUDIO_BUFFER_SIZE 10000
#define SONG_TITLE_SIZE 256
#define SONG_ARTIST_SIZE 256
#define MAX_USERNAME 64
#define MAX_TITLE 64
#define MAX_ARTIST 64
#define MAX_ALBUM 64

#include "defines.h"

enum PktIds {
    ARTWORK,
    JOIN,
    SONG_INFO,
    USERS,
    USER_INFO,
    SONGS,
    SONG_REQUEST,
    SONG_QUEUE,
    USER_ID,
    MUSIC_HEADER,
    HEADER_INFO
};

typedef struct {
    PktIds pktId = PktIds::ARTWORK;
    int len;
    char *buffer;
} Artwork;

typedef struct {
    int pktId = AUDIO;
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
    u_long ip;
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

typedef struct {
    PktIds pktId = PktIds::USER_ID;
    int id;
} UserId;


typedef struct {
    int pkitId = AUDIO_FORMAT_ID;
    char fileType[4];
    qint32 fileSize;
    char waveName[4];
    char fmtName[3];
    qint32 fmtLength;
    short fmtType;
    short numberOfChannels;
    qint32 sampleRate;
    qint32 sampleMulti;
    short bitsMulti;
    short bitsPerSample;
    char dataHeader[4];
    qint32 dataSize;
} HeaderInfo;

#endif
