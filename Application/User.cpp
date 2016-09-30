//
// Created by Erik Nadel on 9/27/16.
//

#include "User.h"

User::User(){

}

User::User(std::string username, TCPStream* stream,  bool isAdmin) {
    this->username = username;
    this->isAdmin = isAdmin;
    this->id = 5;
    this->stream = stream;
}

void User::setActiveChannel(Channel *ch) {
    this->currentChannel = ch;
}

bool User::operator==(User u) {

    if (this->username.compare(u.username) == 0)
        return true;
    else
        return false;

}

void User::sendMessage(string msg){
    this->currentChannel->sendMessage(this,msg);
}


TCPStream* User::getUserStream() {
    return this->stream;
}

bool User::operator!=(User u) {
    if (this->username.compare(u.username) == 0)
        return false;
    else
        return true;
}
