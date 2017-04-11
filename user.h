#ifndef USER_H
#define USER_H

#include <QString>
#include <winsock2.h>
#include <windows.h>

class User
{
public:
    User(QString username, int id, u_long ip)
        : username(username), id(id), ip(ip) {}
    QString getUsername() const {return username;}
    int getId() const {return id;}
    u_long getIp() const {return ip;}
private:
    QString username;
    int id;
    u_long ip;
};

#endif // USER_H
