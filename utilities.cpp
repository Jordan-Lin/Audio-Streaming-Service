/*------------------------------------------------------------------------------
-- SOURCE FILE: utilities.cpp
--
-- PROGRAM: Comm Audio
--
-- FUNCTIONS:
--          void printError(const char *msg, int errCode);
--          void startup();
--          SOCKET createSocket(int type);
--          SOCKET acceptConnection(SOCKET sock);
--          void sendUDP(SOCKET sock, WSABUF& wsaBuf, struct sockaddr_in addr);
--          bool connectSocket(SOCKET sock, struct sockaddr_in addr);
--          void sendTCP(SOCKET sock, WSABUF& wsaBuf);
--          void bindSocket(SOCKET sock, struct sockaddr_in addr);
--          std::pair<SOCKET, struct sockaddr_in> acceptConnectionGetAddr(SOCKET sock);
--          struct sockaddr_in createAddress(u_long ip, int port);
--          void receive(SOCKET sock, WSABUF wsaBuf, LPWSAOVERLAPPED olap, Routine callback);
--          void multicast(SOCKET sock, LPWSABUF wsaBuf, struct sockaddr_in& addr, LPWSAOVERLAPPED overlapped, Routine callback);
--          void joinMulticast(SOCKET sock);
--          void traverseMultiple(SOCKET sock);
--          void disableLoopback(SOCKET sock);
--          QString itoq(int);
--          QString addNull(const char *txt, int len);
--          void handleError(int errCode, const char *msg, ErrorType err);
--          void showWarningMessage(QWidget *parent, QString title, QString msg);
--
-- DATE:    April 10th, 2017
--
-- DESIGNER:    Brody McCrone
--
-- PROGRAMMER:  Brody McCrone
--              Jordan Lin
--
-- NOTES:
--  Utilities file contains general functions that are needed in several places
--  within the program.
------------------------------------------------------------------------------*/

#include "utilities.h"
#include <QDebug>
#include <windows.h>
#include <sstream>
#include "debugwindow.h"
#include <ws2tcpip.h>
#include <QMessageBox>

#define TIMECAST_TTL    2

/*------------------------------------------------------------------------------
-- FUNCTION: startup
--
-- DATE:    April 10th, 2017
--
-- DESIGNER:    Brody McCrone
--
-- PROGRAMMER:  Brody McCrone
--
-- INTERFACE:   void startup();
--
-- PARAMETERS: N/A
--
-- RETURNS: N/A
--
-- NOTES:
--  Wrapper function for WSAStartup
------------------------------------------------------------------------------*/
void startup() {
    WSADATA wsaData;
    int ret = WSAStartup(0x0202, &wsaData);
    if (ret != 0) {
        DebugWindow::get()->logd(QString("WSAStartup failed with error: ") + itoq(ret));
        WSACleanup();
    } else {
        DebugWindow::get()->logd("WSAStartup success.");
    }
}

/*------------------------------------------------------------------------------
-- FUNCTION: createSocket
--
-- DATE:    April 10th, 2017
--
-- DESIGNER:    Brody McCrone
--
-- PROGRAMMER:  Brody McCrone
--
-- INTERFACE:   SOCKET createSocket(int type);
--
-- PARAMETERS: int type     - socket type flag
--
-- RETURNS: SOCKET
--
-- NOTES:
--  Wrapper function for creating a socket
------------------------------------------------------------------------------*/
SOCKET createSocket(int type) {
    SOCKET sock = WSASocket(AF_INET, type, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    if(sock == INVALID_SOCKET) {
        handleError(WSAGetLastError(), "WSASocket", ErrorType::CREATE_SOCKET);
    }
    return sock;
}

/*------------------------------------------------------------------------------
-- FUNCTION: acceptConnection
--
-- DATE:    April 10th, 2017
--
-- DESIGNER:    Brody McCrone
--
-- PROGRAMMER:  Brody McCrone
--
-- INTERFACE:   SOCKET acceptConnection(SOCKET sock);
--
-- PARAMETERS: SOCKET sock      - socket received from listen
--
-- RETURNS: SOCKET
--
-- NOTES:
--  Wrapper function for accepting connection
------------------------------------------------------------------------------*/
SOCKET acceptConnection(SOCKET sock) {
    SOCKET acceptSock = WSAAccept(sock, NULL, NULL, NULL, NULL);
    if (acceptSock == INVALID_SOCKET) {
        handleError(WSAGetLastError(), "acceptConnection, WSAAccept", ErrorType::ACCEPT_CONNECTION);
    }
    return acceptSock;
}

/*------------------------------------------------------------------------------
-- FUNCTION: acceptConnectionGetAddr
--
-- DATE:    April 10th, 2017
--
-- DESIGNER:    Brody McCrone
--
-- PROGRAMMER:  Brody McCrone
--
-- INTERFACE:   std::pair<SOCKET, struct sockaddr_in> acceptConnectionGetAddr(SOCKET sock);
--
-- PARAMETERS: SOCKET sock      - socket received from listen
--
-- RETURNS: pair    -   SOCKET attached to its sockaddr_in struct
--
-- NOTES:
--  function to create a pair matching the received socket to its
--  sock address information struct
------------------------------------------------------------------------------*/
std::pair<SOCKET, struct sockaddr_in> acceptConnectionGetAddr(SOCKET sock) {
    struct sockaddr_in addr;
    INT addrLen = sizeof(addr);
    SOCKET acceptSock = WSAAccept(sock, (sockaddr *)&addr, &addrLen, NULL, NULL);
    if (acceptSock == INVALID_SOCKET) {
        handleError(WSAGetLastError(), "acceptConnection, WSAAccept", ErrorType::ACCEPT_CONNECTION);
    }
    return std::pair<SOCKET, struct sockaddr_in>(acceptSock, addr);
}

/*------------------------------------------------------------------------------
-- FUNCTION: connectSocket
--
-- DATE:    April 10th, 2017
--
-- DESIGNER:    Brody McCrone
--
-- PROGRAMMER:  Brody McCrone
--
-- INTERFACE:   bool connectSocket(SOCKET sock, struct sockaddr_in addr);
--
-- PARAMETERS: N/A
--
-- RETURNS: N/A
--
-- NOTES:
--  Wrapper function for WSAStartup
------------------------------------------------------------------------------*/
bool connectSocket(SOCKET sock, struct sockaddr_in addr) {
    if (connect(sock, (sockaddr *)&addr, sizeof(struct sockaddr_in)) < 0) {
        handleError(WSAGetLastError(), "connectSocket, connect", ErrorType::CONNECT_SOCKET);
        return false;
    }
    return true;
}

/*------------------------------------------------------------------------------
-- FUNCTION: sendTCP
--
-- DATE:    April 10th, 2017
--
-- DESIGNER:    Brody McCrone
--
-- PROGRAMMER:  Brody McCrone
--
-- INTERFACE:   void sendTCP(SOCKET sock, WSABUF buf,
--                          LPWSAOVERLAPPED olap, Routine callback);
--
-- PARAMETERS:  SOCKET sock  -   socket to send to through TCP
--              WSABuf buf   -   buffer to be sent
--              LPWSAOVERLAPPED olap - overlapped structure
--              Routine callback - Callback function for Routine handling
--
-- RETURNS: N/A
--
-- NOTES:
--  Wrapper function to send through TCP with overlapped with completion routine
------------------------------------------------------------------------------*/
void sendTCP(SOCKET sock, WSABUF buf, LPWSAOVERLAPPED olap, Routine callback) {
    DWORD unusedBytesSent = 0;
    if (WSASend(sock, &buf, 1, &unusedBytesSent, 0, olap, callback)) {
        handleError(WSAGetLastError(), "WSASend", ErrorType::SEND_TCP_OLAP);
    }
}

/*------------------------------------------------------------------------------
-- FUNCTION: sendTCP
--
-- DATE:    April 10th, 2017
--
-- DESIGNER:    Brody McCrone
--
-- PROGRAMMER:  Brody McCrone
--
-- INTERFACE:   void sendTCP(SOCKET sock, WSABUF& buf);
--
-- PARAMETERS:  SOCKET sock - socket to send to
--              WSABUG buf - buffer to be sent
--
-- RETURNS: N/A
--
-- NOTES:
--  Wrapper function for regular WSASend through TCP
------------------------------------------------------------------------------*/
void sendTCP(SOCKET sock, WSABUF& buf) {
    DWORD unusedBytesSent = 0;
    if (WSASend(sock, &buf, 1, &unusedBytesSent, 0, NULL, NULL)) {
        handleError(WSAGetLastError(), "WSASend", ErrorType::SEND_TCP);
    }
}

/*------------------------------------------------------------------------------
-- FUNCTION: bindSocket
--
-- DATE:    April 10th, 2017
--
-- DESIGNER:    Brody McCrone
--
-- PROGRAMMER:  Brody McCrone
--
-- INTERFACE:   void bindSocket(SOCKET sock, struct sockaddr_in addr);
--
-- PARAMETERS:  SOCKET sock - socket to be bound
--              struct sockaddr_in addr - socket address structure
--
-- RETURNS: N/A
--
-- NOTES:
--  Wrapper function for binding a socket.
------------------------------------------------------------------------------*/
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

/*------------------------------------------------------------------------------
-- FUNCTION: createAddress
--
-- DATE:    April 10th, 2017
--
-- DESIGNER:    Brody McCrone
--
-- PROGRAMMER:  Brody McCrone
--
-- INTERFACE:   struct sockaddr_in createAddress(u_long ip, int port);
--
-- PARAMETERS:  u_long ip - the socket IP as an unsigned long
--              int port - the socket port
--
-- RETURNS: struct sockaddr_in
--
-- NOTES:
--  Wrapper function for creating a sockaddr_in struct
------------------------------------------------------------------------------*/
struct sockaddr_in createAddress(u_long ip, int port) {
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = port;
    addr.sin_addr.s_addr = ip;
    return addr;
}

/*------------------------------------------------------------------------------
-- FUNCTION: receive
--
-- DATE:    April 10th, 2017
--
-- DESIGNER:    Brody McCrone
--
-- PROGRAMMER:  Brody McCrone
--
-- INTERFACE:   void receive(SOCKET sock, WSABUF wsaBuf,
--                              LPWSAOVERLAPPED olap, Routine callback);
--
-- PARAMETERS:  SOCKET sock  -   socket to receive from using TCP
--              WSABuf buf   -   buffer to be sent
--              LPWSAOVERLAPPED olap - overlapped structure
--              Routine callback - Callback function for Routine handling
--
-- RETURNS: N/A
--
-- NOTES:
--  Wrapper function to receive using overlapped and completion routines
------------------------------------------------------------------------------*/
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
/*------------------------------------------------------------------------------
-- FUNCTION: multicast
--
-- DATE:    April 10th, 2017
--
-- DESIGNER:    Brody McCrone
--
-- PROGRAMMER:  Brody McCrone
--
-- INTERFACE:   void multicast(SOCKET sock, LPWSABUF wsaBuf, struct sockaddr_in& addr,
 --                             LPWSAOVERLAPPED overlapped, Routine callback)
--
-- PARAMETERS:  SOCKET sock  -   socket to send to through multicast
--              LPWSABUF - pointer to the WSA Bffer
--              struct sockaddr_in& addr - reference to a sock addr struct
--                                         to send to.
--              LPWSAOVERLAPPED olap - overlapped structure
--              Routine callback - Callback function for Routine handling
--
-- RETURNS: N/A
--
-- NOTES:
--  Wrapper function for multicasting to multiple clients
------------------------------------------------------------------------------*/
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

/*------------------------------------------------------------------------------
-- FUNCTION: sendUDP
--
-- DATE:    April 10th, 2017
--
-- DESIGNER:    Brody McCrone
--
-- PROGRAMMER:  Brody McCrone
--
-- INTERFACE:   void sendUDP(SOCKET sock, WSABUF& wsaBuf, struct sockaddr_in addr);
--
-- PARAMETERS:  SOCKET sock  -   socket to send to through UDP
--              WSABUF& wsaBuf   -   buffer to be sent
--              struct sockaddr_in& addr - reference to a sock addr struct
--                                         to send to.
--
-- RETURNS: N/A
--
-- NOTES:
--  Wrapper function to send through a UDP socket
------------------------------------------------------------------------------*/
void sendUDP(SOCKET sock, WSABUF& wsaBuf, struct sockaddr_in addr) {
    DWORD unusedSendBytes = 0;
    if (WSASendTo(sock, &wsaBuf, 1, &unusedSendBytes, 0, (struct sockaddr *)&addr,
            sizeof(struct sockaddr_in), NULL, NULL)) {
        handleError(WSAGetLastError(), "WSASend", ErrorType::SEND_UDP);
    }
}

/*------------------------------------------------------------------------------
-- FUNCTION: joinMulticast
--
-- DATE:    April 10th, 2017
--
-- DESIGNER:    Brody McCrone
--
-- PROGRAMMER:  Brody McCrone
--
-- INTERFACE:   void joinMulticast(SOCKET sock);
--
-- PARAMETERS:  SOCKET sock  -   socket being added to the multicast
--
-- RETURNS: N/A
--
-- NOTES:
--  Wrapper function to add a socket to the multicasting server
------------------------------------------------------------------------------*/
void joinMulticast(SOCKET sock) {
    struct ip_mreq multicastInterface;
    multicastInterface.imr_multiaddr.s_addr = inet_addr("234.5.6.8");
    multicastInterface.imr_interface.s_addr = INADDR_ANY;
    int setSockOptRet = setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP,
        (char *)&multicastInterface, sizeof(multicastInterface));
    if (setSockOptRet == SOCKET_ERROR) {
        handleError(WSAGetLastError(), "setsockopt IP_ADD_MEMBERSHIP", ErrorType::JOIN_MULTICAST);
    }
}

/*------------------------------------------------------------------------------
-- FUNCTION: traverseMultiple
--
-- DATE:    April 10th, 2017
--
-- DESIGNER:    Brody McCrone
--
-- PROGRAMMER:  Brody McCrone
--
-- INTERFACE:   void traverseMultiple(SOCKET sock);
--
-- PARAMETERS:  SOCKET sock  -   socket who's options are being changed.
--
-- RETURNS: N/A
--
-- NOTES:
--  Wrapper function to set the socket options
------------------------------------------------------------------------------*/
void traverseMultiple(SOCKET sock) {
    u_long lTTL = TIMECAST_TTL;
    int ret = setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL, (char *)&lTTL, sizeof(lTTL));
    if (ret == SOCKET_ERROR) {
        handleError(WSAGetLastError(), "setsockopt IP_MULTICAST_TTL",  ErrorType::TRAVERSE_MULTIPLE);
    }
}

/*------------------------------------------------------------------------------
-- FUNCTION: disableLoopback
--
-- DATE:    April 10th, 2017
--
-- DESIGNER:    Brody McCrone
--
-- PROGRAMMER:  Brody McCrone
--
-- INTERFACE:   void disableLoopback(SOCKET sock);
--
-- PARAMETERS:  SOCKET sock  -   Socket to be disabled
--
-- RETURNS: N/A
--
-- NOTES:
--  Wrapper function to disable the socket loopback
------------------------------------------------------------------------------*/
void disableLoopback(SOCKET sock) {
    bool flag = FALSE;
    if (setsockopt(sock, IPPROTO_IP, IP_MULTICAST_LOOP, (char *)&flag, sizeof(flag)) == SOCKET_ERROR) {
        handleError(WSAGetLastError(), "setsockopt IP_MULTICAST_TTL",  ErrorType::DISABLE_LOOPBACK);
    }
}

/*------------------------------------------------------------------------------
-- FUNCTION: printError
--
-- DATE:    April 10th, 2017
--
-- DESIGNER:    Brody McCrone
--
-- PROGRAMMER:  Brody McCrone
--
-- INTERFACE:   void printError(const char *msg, int errCode);
--
-- PARAMETERS:  const char *msg - error message
--              int errCode - error code
--
-- RETURNS: N/A
--
-- NOTES:
--  Wrapper function to print an error.
------------------------------------------------------------------------------*/
void printError(const char *msg, int errCode) {
    wchar_t *s = NULL;
    FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                   NULL, errCode,
                   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                   (LPWSTR)&s, 0, NULL);
    DebugWindow::get()->logd(QString(msg) + QString(": ") + QString::fromWCharArray(s) + QString(", errCode: ") + itoq(errCode));
    LocalFree(s);
}

/*------------------------------------------------------------------------------
-- FUNCTION: handleError
--
-- DATE:    April 10th, 2017
--
-- DESIGNER:    Brody McCrone
--
-- PROGRAMMER:  Brody McCrone
--
-- INTERFACE:   void handleError(int errCode, const char *msg, ErrorType err);
--
-- PARAMETERS:  int errCode - error code
--              const char *msg - error message
--              ErrorType err - error type struct
--
-- RETURNS: N/A
--
-- NOTES:
--  Error processing.
------------------------------------------------------------------------------*/
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
    case ErrorType::SEND_TCP:
        break;
    case ErrorType::SEND_TCP_OLAP:
        break;
    case ErrorType::ACCEPT_CONNECTION:
        break;
    case ErrorType::CONNECT_SOCKET:
        break;
    case ErrorType::SEND_UDP:
        break;
    }
}

/*------------------------------------------------------------------------------
-- FUNCTION: itoq
--
-- DATE:    April 10th, 2017
--
-- DESIGNER:    Brody McCrone
--
-- PROGRAMMER:  Brody McCrone
--
-- INTERFACE:   QString itoq(int n);
--
-- PARAMETERS:  int n - integer to be converted
--
-- RETURNS: QString
--
-- NOTES:
--  convert integer to a QString
------------------------------------------------------------------------------*/
QString itoq(int n) {
    std::ostringstream oss;
    oss << n;
    return QString::fromStdString(oss.str());
}

/*------------------------------------------------------------------------------
-- FUNCTION: sendUDP
--
-- DATE:    April 10th, 2017
--
-- DESIGNER:    Brody McCrone
--
-- PROGRAMMER:  Brody McCrone
--
-- INTERFACE:   QString addNull(const char *txt, int len);
--
-- PARAMETERS:  const char *txt - text to append nulls to
--              int len - length of the character array
--
-- RETURNS: QString
--
-- NOTES:
--  Add nulls to a QString
------------------------------------------------------------------------------*/
QString addNull(const char *txt, int len) {
    char *temp = (char *)malloc(len + 1);
    strncpy(temp, txt, len + 1);
    temp[len] = '\0';
    QString q(temp);
    free(temp);
    return q;
}

/*------------------------------------------------------------------------------
-- FUNCTION: showWarningMessage
--
-- DATE:    April 10th, 2017
--
-- DESIGNER:    Jordan Lin
--
-- PROGRAMMER:  Jordan Lin
--
-- INTERFACE:   void showWarningMessage(QWidget * parent, QString title, QString msg);
--
-- PARAMETERS:  QWidget * parent - parent widget spawning the message box
--              QString title - title of the message box
--              QString msg - message context.
--
-- RETURNS: N/A
--
-- NOTES:
--  Message box pop up function.
------------------------------------------------------------------------------*/
void showWarningMessage(QWidget * parent, QString title, QString msg)
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::information(parent, title, msg);
}
