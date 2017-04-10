#include "client.h"
#include "utilities.h"
#include "packets.h"
#include <thread>
#include "MainWindow.h"

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
        MainWindow::get()->logd("Failed to connect to server.");
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
        MainWindow::get()->logd("Client::receiveRoutine failed.");
    }

    reinterpret_cast<ClientOlap *>(olap)->client->parse(recvBytes);
}

void Client::parse(int recvBytes) {
    PktIds *pktId = reinterpret_cast<PktIds *>(buffer);
    while (recvBytes) {
        switch (*pktId) {
        case PktIds::USERS:
            {
                UserInfo *start = reinterpret_cast<UserInfo *>(buffer + sizeof(PktIds::USERS) + sizeof(int));
                UserInfo *end = reinterpret_cast<UserInfo *>(
                            start + *reinterpret_cast<int *>(
                            buffer + sizeof(PktIds::USERS)));
                while (start != end) {
                    MainWindow::get()->logd(QString("Client id: ") + start->userId);
                    MainWindow::get()->logd(QString("Client name: ") + start->username);
                    recvBytes -= sizeof(UserInfo);
                    ++start;
                }
                recvBytes -= (sizeof(PktIds::USERS) + sizeof(int));
            }
        }
    }

    wsaBuf.buf = buffer;
    wsaBuf.len = sizeof(buffer);
    receive(sock, wsaBuf, &olapWrap.olap, receiveRoutine);
}
