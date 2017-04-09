#ifndef ROUTINESTRUCTS_H
#define ROUTINESTRUCTS_H

#include <winsock2.h>
#include <windows.h>
#include <fstream>
class SongStreamReceiver;
class SongStreamer;
class Client;

#define RECEIVE_BUFFER_SIZE 1024
#define SEND_SONG_BUFFER_SIZE 1024

typedef struct {
    OVERLAPPED overlapped;
    SOCKET sock;
} OverlappedSocket;

typedef struct {
    OverlappedSocket overlappedSocket;
    WSABUF buf;
} ReceiveData;

typedef struct {
    ReceiveData receiveData;
    char buffer[RECEIVE_BUFFER_SIZE];
} ReceiveSongStream;

typedef struct {
    OverlappedSocket overlappedSocket;
    struct sockaddr_in addr;
    std::ifstream file;
    char buffer[SEND_SONG_BUFFER_SIZE];
    bool finished = false;
} SendSongStream;

typedef struct {
    WSAOVERLAPPED olap;
    SongStreamReceiver *receiver;
} SongStreamReceiverOlapWrap;

typedef struct {
    WSAOVERLAPPED olap;
    SongStreamer *sender;
} SongStreamerOlapWrap;

typedef struct {
    WSAOVERLAPPED olap;
    Client *client;
}

#endif // ROUTINESTRUCTS_H
