//
// Created by Erik Nadel on 9/27/16.
//

#ifndef PROJECT1_CLIENTQUEUE_H
#define PROJECT1_CLIENTQUEUE_H


#include <queue>
#include "User.h"

class ClientQueue {

public:
    ClientQueue();
    void addUser(User *u);
    User* getNext();
    int getSize();
    int getChattyUsers();

private:
    std::vector<User*> users;

};


#endif //PROJECT1_CLIENTQUEUE_H
