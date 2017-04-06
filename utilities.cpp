#include "utilities.h"
#include <QDebug>
#include <windows.h>
#include <sstream>
#include "mainwindow.h"
#include <ws2tcpip.h>


#define TIMECAST_TTL    2

void startup() {
    WSADATA wsaData;
    int ret = WSAStartup(0x0202, &wsaData);
    if (ret != 0) {
        MainWindow::get()->logd(QString("WSAStartup failed with error: ") + itoq(ret));
        WSACleanup();
    } else {
        MainWindow::get()->logd("WSAStartup success.");
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
    bool flag = TRUE;
    int ret = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&flag, sizeof(flag));
    if (ret == SOCKET_ERROR) {
        handleError(WSAGetLastError(), "SO_REUSEADDR", ErrorType::SET_REUSABLE);
    }

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

void receive(SOCKET sock, WSABUF wsaBuf, LPWSAOVERLAPPED olap, Routine callback) {
    DWORD flags = 0;
    DWORD unusedRecvBytes = 0;
    memset(olap, 0, sizeof(WSAOVERLAPPED));
    if (WSARecv(sock, &wsaBuf, 1, &unusedRecvBytes, &flags, olap, callback) == SOCKET_ERROR) {
        int err = WSAGetLastError();
        if (err != WSA_IO_PENDING) {
            handleError(err, "WSARecv", ErrorType::RECEIVE);
        }
    }
}

void multicast(SOCKET sock, LPWSABUF wsaBuf, struct sockaddr_in& addr, LPWSAOVERLAPPED overlapped, Routine callback) {
    DWORD unusedSendBytes = 0;
    memset(overlapped, 0, sizeof(WSAOVERLAPPED));
    if (WSASendTo(sock, wsaBuf, 1, &unusedSendBytes, 0, (struct sockaddr *)&addr,
            sizeof(struct sockaddr_in), overlapped, callback) == SOCKET_ERROR) {
        int err = WSAGetLastError();
        if (err != WSA_IO_PENDING) {
            handleError(err, "WSASend", ErrorType::SEND);
        }
    }

}

void joinMulticast(SOCKET sock) {
    struct ip_mreq multicastInterface;
    multicastInterface.imr_multiaddr.s_addr = inet_addr("234.5.6.7");
    multicastInterface.imr_interface.s_addr = INADDR_ANY;
    int setSockOptRet = setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP,
        (char *)&multicastInterface, sizeof(multicastInterface));
    if (setSockOptRet == SOCKET_ERROR) {
        handleError(WSAGetLastError(), "setsockopt IP_ADD_MEMBERSHIP", ErrorType::JOIN_MULTICAST);
    }
}

void traverseMultiple(SOCKET sock) {
    u_long lTTL = TIMECAST_TTL;
    int ret = setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL, (char *)&lTTL, sizeof(lTTL));
    if (ret == SOCKET_ERROR) {
        handleError(WSAGetLastError(), "setsockopt IP_MULTICAST_TTL",  ErrorType::TRAVERSE_MULTIPLE);
    }
}

void disableLoopback(SOCKET sock) {
    bool flag = FALSE;
    if (setsockopt(sock, IPPROTO_IP, IP_MULTICAST_LOOP, (char *)&flag, sizeof(flag)) == SOCKET_ERROR) {
        handleError(WSAGetLastError(), "setsockopt IP_MULTICAST_TTL",  ErrorType::DISABLE_LOOPBACK);
    }
}

void printError(const char *msg, int errCode) {
    wchar_t *s = NULL;
    FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                   NULL, errCode,
                   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                   (LPWSTR)&s, 0, NULL);
    MainWindow::get()->logd(QString(msg) + QString(": ") + QString::fromWCharArray(s) + QString(", errCode: ") + itoq(errCode));
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
    case ErrorType::JOIN_MULTICAST:
        break;
    case ErrorType::TRAVERSE_MULTIPLE:
        break;
    case ErrorType::DISABLE_LOOPBACK:
        break;
    case ErrorType::SET_REUSABLE:
        break;
    }
}

QString itoq(int n) {
    std::ostringstream oss;
    oss << n;
    return QString::fromStdString(oss.str());
}

QString addNull(const char *txt, int len) {
    char *temp = (char *)malloc(len + 1);
    strncpy(temp, txt, len + 1);
    temp[len] = '\0';
    QString q(temp);
    free(temp);
    return q;
}
