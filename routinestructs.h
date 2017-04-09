#ifndef ROUTINESTRUCTS_H
#define ROUTINESTRUCTS_H

#include <winsock2.h>
#include <windows.h>

class SongStreamReceiver;
class SongStreamer;
class ClientHandler;
class Client;

#define RECEIVE_BUFFER_SIZE 1024
#define SEND_SONG_BUFFER_SIZE 1024



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
    ClientHandler *client;
} ClientHandlerOlap;

typedef struct {
    WSAOVERLAPPED olap;
    Client *client;
} ClientOlap;

#endif // ROUTINESTRUCTS_H
