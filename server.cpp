
#include "server.h"
#include "clienthandler.h"
#include "clientmanager.h"
#include <thread>
#include "utilities.h"
#include "songmanager.h"

Server::Server() {
    state = ServerState::INITIALIZING;
    std::thread serverThread(&Server::run, this);
    serverThread.detach();
}

void Server::run() {
    listenSock = createSocket(SOCK_STREAM);
    bindSocket(listenSock, createAddress(htonl(INADDR_ANY), htons(REQUEST_PORT)));
    listen(listenSock, 5);

    addDummySongs();
    state = ServerState::RUNNING;
    for(;;) {
        //attatched to shared_ptr once Join packet is received.
        ClientHandler *client = new ClientHandler(acceptConnection(listenSock));
    }
}

void Server::addDummySongs() {
    Song song(SongManager::get().genId(), "Yolo", "GG", "reported");
    Song song1(SongManager::get().genId(), "Title", "Artist", "Album");
    Song song2(SongManager::get().genId(), "The", "Best", "Song");
    Song song3(SongManager::get().genId(), "One", "Two", "Three");

    SongManager::get().addSong(song);
    SongManager::get().addSong(song1);
    SongManager::get().addSong(song2);
    SongManager::get().addSong(song3);

}
