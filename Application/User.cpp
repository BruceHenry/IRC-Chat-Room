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
    this->requestedChat = false;
}

void User::setChatStatus(bool stat) {
    this->requestedChat = stat;
}

bool User::getChatStatus() {
    return this->requestedChat;
}

void User::setActiveChannel(Channel *ch) {
    this->currentChannel = ch;
}

Channel* User::getActiveChannel() {
    return this->currentChannel;
}

bool User::operator==(User u) {

    if (this->username.compare(u.username) == 0)
        return true;
    else
        return false;

}

void User::sendMessage(string msg){
    if(this->currentChannel == NULL) {
        string error = "ERROR NOT_IN_CHANNEL";
        this->stream->send(error.c_str(), error.length());
    }
    this->currentChannel->sendMessage(this,msg);
}

std::string User::getUsername() {
    return this->username;
}


TCPStream* User::getUserStream() {
    return this->stream;
}

void User::leaveChannel() {
    this->currentChannel = NULL;
}

bool User::operator!=(User u) {
    if (this->username.compare(u.username) == 0)
        return false;
    else
        return true;
}
