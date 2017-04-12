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
#include <QIODevice>
#include <QTextStream>
#include <QFile>

Client::Client(QString serverIp, QString username) {
    std::string strServerIp(serverIp.toStdString());
    const char *cStrServerIp = strServerIp.c_str();

    servAddr = createAddress(inet_addr(cStrServerIp), htons(REQUEST_PORT));
    olapWrap.client = this;
    sock = createSocket(SOCK_STREAM);

    std::thread clientThread(&Client::run, this, username);
    clientThread.detach();

    receiver = new SongStreamReceiver(6555);
    std::thread receiverThread(&SongStreamReceiver::init, receiver);
    receiverThread.detach();
}

Client::~Client() {
    delete receiver;
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
        offset = 0;

        PktIds *pktId = reinterpret_cast<PktIds *>(tempBuffer);
        switch (*pktId) {
        case PktIds::USERS:
            {
                UserInfo *start = reinterpret_cast<UserInfo *>(tempBuffer + sizeof(PktIds::USERS) + sizeof(int));
                UserInfo *end = reinterpret_cast<UserInfo *>(
                            start + *reinterpret_cast<int *>(
                            tempBuffer + sizeof(PktIds::USERS)));
                while (start != end) {
                    DebugWindow::get()->logd(QString("New user ip: ") + itoq(start->ip));
                    User user(start->username, start->userId, start->ip);\
                    if (start->userId != id) {
                        UserManager::get().insert(start->userId, user);
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
                int size = SongQueue::get().size();
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
        case PktIds::DOWNLOAD:
            {
                int songId = *reinterpret_cast<int *>(tempBuffer + sizeof(PktIds::DOWNLOAD));
                int len = *reinterpret_cast<int *>(tempBuffer + sizeof(PktIds::DOWNLOAD) + sizeof(int));
                QByteArray data(tempBuffer + sizeof(PktIds::DOWNLOAD) + sizeof(int) + sizeof(int),
                        recvBytes - sizeof(PktIds::DOWNLOAD) - sizeof(int) - sizeof(int));
                len -= (recvBytes - sizeof(PktIds::DOWNLOAD) - sizeof(int) - sizeof(int));
                while (len) {
                    wsaBuf.buf = buffer;
                    if (len > sizeof(buffer))
                        wsaBuf.len = sizeof(buffer);
                    else
                        wsaBuf.len = len;
                    int ret = recvTCP(sock, wsaBuf);
                    data.append(wsaBuf.buf, ret);
                    len -= ret;
                }
                QString fileName(SongManager::get().at(songId).getTitle() + ".wav");
                QFile file(fileName);
                if (file.open(QIODevice::ReadWrite)) {
                    DebugWindow::get()->logd("writing to file");
                    QTextStream stream(&file);
                    stream << data;
                } else {
                    DebugWindow::get()->logd("failed to file");

                }
                file.close();
                recvBytes = 0;
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
    request.songId = songId + 1;
    WSABUF wsaBuf;
    wsaBuf.buf = reinterpret_cast<char *>(&request);
    wsaBuf.len = sizeof(SongRequest);

    sendTCP(sock, wsaBuf);
}

void Client::sendDownloadRequest(int songId) {
    DebugWindow::get()->logd("Sending download request");
    DownloadRequest request;
    request.songId = songId + 1;
    WSABUF wsaBuf;
    wsaBuf.buf = reinterpret_cast<char *>(&request);
    wsaBuf.len = sizeof(DownloadRequest);

    sendTCP(sock, wsaBuf);
}
