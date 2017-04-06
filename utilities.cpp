#include "utilities.h"
#include <QDebug>
#include <windows.h>
#include <sstream>

void startup() {
    WSADATA wsaData;
    int ret = WSAStartup(0x0202, &wsaData);
    if (ret != 0) {
        qDebug() << "WSAStartup failed with error: " << ret;
        WSACleanup();
    } else {
        qDebug() << "WSAStartup success.";
    }
}

SOCKET createSocket(int type) {
    SOCKET sock = WSASocket(AF_INET, type, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    if(sock == INVALID_SOCKET) {
        handleError(WSAGetLastError(), "WSASocket", ErrorType::CREATE_SOCKET);
    }
    return sock;
}

void bindSocket(SOCKET sock, struct sockaddr_in addr) {
    if (bind(sock, reinterpret_cast<struct sockaddr *>(&addr), sizeof(struct sockaddr_in))) {
        handleError(WSAGetLastError(), "bind", ErrorType::BIND_SOCKET);
    }
}

struct sockaddr_in createAddress(u_long ip, int port) {
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = port;
    addr.sin_addr.s_addr = ip;
    return addr;
}

void receive(SOCKET sock, WSABUF wsaBuf, OVERLAPPED& olap, Routine callback) {
    DWORD flags = 0;
    DWORD unusedRecvBytes;
    memset(&olap, 0, sizeof(OVERLAPPED));
    if (WSARecv(sock, &wsaBuf, 1, &unusedRecvBytes, &flags, &olap, callback) == SOCKET_ERROR) {
        int err = WSAGetLastError();
        if (err != WSA_IO_PENDING) {
            handleError(err, "WSARecv", ErrorType::RECEIVE);
        }
    }
}

void multicast(SOCKET sock, WSABUF& wsaBuf, struct sockaddr_in& addr, OVERLAPPED& overlapped, Routine callback) {
    DWORD unusedSendBytes;
    if (WSASendTo(sock, &wsaBuf, 1, &unusedSendBytes, 0, (struct sockaddr *)&addr,
            sizeof(struct sockaddr_in), &overlapped, callback) == SOCKET_ERROR) {
        int err = WSAGetLastError();
        if (err != WSA_IO_PENDING) {
            handleError(err, "WSASend", ErrorType::SEND);
        }
    }
}

void printError(const char *msg, int errCode) {
    wchar_t *s = NULL;
    FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                   NULL, errCode,
                   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                   (LPWSTR)&s, 0, NULL);
    qDebug() << msg << ": " << QString::fromWCharArray(s) << ", errCode: " << errCode;
    LocalFree(s);
}

void handleError(int errCode, const char *msg, ErrorType err) {
    printError(msg, errCode);
    switch(err) {
    case ErrorType::CREATE_SOCKET:
        break;
    case ErrorType::RECEIVE:
        break;
    case ErrorType::BIND_SOCKET:
        break;
    }
}

QString itoq(int n) {
    std::ostringstream oss;
    oss << n;
    return QString::fromStdString(oss.str());
}
