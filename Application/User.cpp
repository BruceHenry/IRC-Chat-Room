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

bool User::operator==(User u) {

    if (this->username.compare(u.username) == 0)
        return true;
    else
        return false;

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
