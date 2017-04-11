#include "client.h"
#include "utilities.h"
#include "packets.h"
#include <thread>
#include "debugwindow.h"
#include "updatehandler.h"
#include "usermanager.h"
#include "debugwindow.h"
#include "songmanager.h"
#include "songqueue.h"

Client::Client(QString serverIp, QString username) {
    std::string strServerIp(serverIp.toStdString());
    const char *cStrServerIp = strServerIp.c_str();

    servAddr = createAddress(inet_addr(cStrServerIp), htons(REQUEST_PORT));
    olapWrap.client = this;
    sock = createSocket(SOCK_STREAM);

    std::thread clientThread(&Client::run, this, username);
    clientThread.detach();
}

void Client::run(QString username) {
    if (connectSocket(sock, servAddr) == false) {
        DebugWindow::get()->logd("Failed to connect to server.");
    } else {
        Join join;
        std::string strUsername(username.toStdString());
        const char *cStrUsername = strUsername.c_str();
        memcpy(join.username, cStrUsername, strlen(cStrUsername) + 1);
        wsaBuf.buf = reinterpret_cast<char *>(&join);
        wsaBuf.len = sizeof(Join);
        sendTCP(sock, wsaBuf);

        wsaBuf.len = BUFFER_SIZE;
        wsaBuf.buf = buffer;
        receive(sock, wsaBuf, &olapWrap.olap, receiveRoutine);
        while (true) {
            SleepEx(INFINITE, TRUE);
        }
    }
}

void Client::receiveRoutine(DWORD errCode, DWORD recvBytes, LPOVERLAPPED olap, DWORD flags) {
    if (errCode != 0) {
        DebugWindow::get()->logd("Client::receiveRoutine failed.");
    }

    reinterpret_cast<ClientOlap *>(olap)->client->parse(recvBytes);
}

void Client::parse(int recvBytes) {
    char *tempBuffer = buffer;
    int offset = 0;
    while (recvBytes) {
        tempBuffer += offset;
        PktIds *pktId = reinterpret_cast<PktIds *>(tempBuffer);
        switch (*pktId) {
        case PktIds::USERS:
            {
                UserInfo *start = reinterpret_cast<UserInfo *>(tempBuffer + sizeof(PktIds::USERS) + sizeof(int));
                UserInfo *end = reinterpret_cast<UserInfo *>(
                            start + *reinterpret_cast<int *>(
                            tempBuffer + sizeof(PktIds::USERS)));
                while (start != end) {
                    User user(start->username, start->userId);
                    if (start->userId != id) {
                        UserManager::get().insert(start->userId, user);
                        DebugWindow::get()->logd(QString("User id: ") + itoq(start->userId) +
                                QString(", User name: ") + start->username);
                    }
                    offset += sizeof(UserInfo);
                    ++start;
                }
                offset += (sizeof(PktIds::USERS) + sizeof(int));
                UpdateHandler::get()->emitUUV();
            }
            break;
        case PktIds::SONGS:
            {
                int songOffset = sizeof(PktIds::SONGS);
                int numSongs = *reinterpret_cast<int *>(tempBuffer + songOffset);
                songOffset += sizeof(int);
                while (numSongs--) {
                    songOffset += sizeof(PktIds::SONG_INFO);
                    int songId = *reinterpret_cast<int *>(tempBuffer + songOffset);
                    songOffset += sizeof(int);

                    int titleLen = *reinterpret_cast<int *>(tempBuffer + songOffset);
                    songOffset += sizeof(int);
                    QString title(tempBuffer + songOffset);
                    songOffset += titleLen;

                    int artistLen = *reinterpret_cast<int *>(tempBuffer + songOffset);
                    songOffset += sizeof(int);
                    QString artist(tempBuffer + songOffset);
                    songOffset += artistLen;

                    int albumLen = *reinterpret_cast<int *>(tempBuffer + songOffset);
                    songOffset += sizeof(int);
                    QString album(tempBuffer + songOffset);
                    songOffset += albumLen;

                    songOffset += sizeof(int);
                    Song song(songId, title, artist, album);
                    SongManager::get().insert(songId, song);
                }
                offset += songOffset;
                UpdateHandler::get()->emitUSV();
            }
            break;
        case PktIds::SONG_QUEUE:
            {
                SongQueue::get().empty();
                int *start = reinterpret_cast<int *>(tempBuffer + sizeof(PktIds::SONG_QUEUE) +
                        sizeof(int));
                int *end = reinterpret_cast<int *>(
                        start + *reinterpret_cast<int *>(
                        tempBuffer + sizeof(PktIds::SONG_QUEUE)));
                while (start != end) {
                    if (SongManager::get().exists(*start)) {
                        SongQueue::get().push(SongManager::get().at(*start));
                    }
                    offset += sizeof(int);
                    ++start;
                }
                offset += (sizeof(PktIds::SONG_QUEUE) + sizeof(int));
                UpdateHandler::get()->emitUQV();
            }
            break;
        case PktIds::USER_ID:
            {
                UserId *pktId = reinterpret_cast<UserId *>(tempBuffer);
                id = pktId->id;
                offset += (sizeof(UserId));
            }
            break;
        }
        recvBytes -= offset;
    }

    memset(buffer, 0, sizeof(buffer));
    wsaBuf.buf = buffer;
    wsaBuf.len = sizeof(buffer);
    receive(sock, wsaBuf, &olapWrap.olap, receiveRoutine);
}

void Client::sendSongRequest(int songId) {
    SongRequest request;
    request.songId = songId;
    WSABUF wsaBuf;
    wsaBuf.buf = reinterpret_cast<char *>(&request);
    wsaBuf.len = sizeof(wsaBuf);

    sendTCP(sock, wsaBuf);
}