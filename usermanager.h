#ifndef USERMANAGER_H
#define USERMANAGER_H

#include "user.h"
#include "lockedmap.h"

class UserManager : public LockedMap<int, User> {
public:
    static UserManager& get() {return instance;}
private:
    static UserManager instance;
};

#endif // USERMANAGER_H
