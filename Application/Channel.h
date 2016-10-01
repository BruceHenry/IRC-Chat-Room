//
// Created by Erik Nadel on 9/27/16.
//

#ifndef PROJECT1_CHANNEL_H
#define PROJECT1_CHANNEL_H


#include <string>
#include <vector>
#include "User.h"

class User;

class Channel {
public:
    Channel();
    void addUser(User* u);
    void sendMessage(User* u, std::string message);
    void removeUser(User* u);
    std::vector<User*> getUsers();
    void transferFile(User* u, std::string filename,long filesize);

private:
    std::vector<User*> users;

};


#endif //PROJECT1_CHANNEL_H
