//
// Created by Erik Nadel on 9/27/16.
//

#ifndef PROJECT1_USER_H
#define PROJECT1_USER_H


#include <string>
#include "../TCP/TCPStream.h"
#include "Channel.h"

class Channel;

class User {
public:
    User();
    User(std::string username, TCPStream* stream,bool isAdmin);
    bool operator==(User u);
    bool operator!=(User u);
    void setActiveChannel(Channel *ch);
    TCPStream* getUserStream();
    void sendMessage(string msg);

private:
    int id;
    bool isAdmin;
    std::string username;
    TCPStream *stream;
    Channel *currentChannel;
};


#endif //PROJECT1_USER_H
