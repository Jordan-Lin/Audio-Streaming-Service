#ifndef USER_H
#define USER_H

#include <QString>

class User
{
public:
    User(QString username, int id)
        : username(username), id(id) {}
    QString getUsername() const {return username;}
private:
    QString username;
    int id;
};

#endif // USER_H
