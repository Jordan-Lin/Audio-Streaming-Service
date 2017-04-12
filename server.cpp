
#include "server.h"
#include "clienthandler.h"
#include "clientmanager.h"
#include <thread>
#include "utilities.h"
#include "songmanager.h"
#include "debugwindow.h"

Server::Server() {
    state = ServerState::INITIALIZING;
    std::thread serverThread(&Server::run, this);
    serverThread.detach();

    streamer = new SongStreamer();
    std::thread streamerThread(&SongStreamer::initStream, streamer);
    streamerThread.detach();
}

void Server::run() {
    listenSock = createSocket(SOCK_STREAM);
    bindSocket(listenSock, createAddress(htonl(INADDR_ANY), htons(REQUEST_PORT)));
    listen(listenSock, 5);

    //addDummySongs();
    state = ServerState::RUNNING;
    for(;;) {
        //attatched to shared_ptr once Join packet is received.
        std::pair<SOCKET, struct sockaddr_in> sockAddr = acceptConnectionGetAddr(listenSock);
        DebugWindow::get()->logd(QString("rece3ived ip: ") + itoq(sockAddr.second.sin_addr.s_addr));
        ClientHandler *client = new ClientHandler(sockAddr.first, sockAddr.second.sin_addr.s_addr);
    }
}

void Server::addDummySongs() {
//    Song song(SongManager::get().genId(), "Yolo", "GG", "reported");
//    Song song1(SongManager::get().genId(), "Title", "Artist", "Album");
//    Song song2(SongManager::get().genId(), "The", "Best", "Song");
//    Song song3(SongManager::get().genId(), "One", "Two", "Three");

//    SongManager::get().addSong(song);
//    SongManager::get().addSong(song1);
//    SongManager::get().addSong(song2);
//    SongManager::get().addSong(song3);

}
