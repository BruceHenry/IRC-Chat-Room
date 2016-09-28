//
// Created by Erik Nadel on 9/27/16.
//

#ifndef PROJECT1_USER_H
#define PROJECT1_USER_H


#include <string>
#include "../TCP/TCPStream.h"

class User {
public:
    User();
    User(std::string username, TCPStream* stream,bool isAdmin);
    bool operator==(User u);
    bool operator!=(User u);
    TCPStream* getUserStream();

private:
    int id;
    bool isAdmin;
    std::string username;
    TCPStream *stream;
};


#endif //PROJECT1_USER_H
