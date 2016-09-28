//
// Created by Erik Nadel on 9/27/16.
//

#include "Channel.h"

Channel::Channel() {
    users = std::vector<User*>();
    messages = std::vector<Message>();

}

void Channel::addUser(User* u) {
    this->users.push_back(u);
}


void Channel::sendMessage(User* sender, std::string message) {

    for(User* u : users){
        if(*u != *sender){
            TCPStream* s = u->getUserStream();
            s->send(message.c_str(),message.length());
        }
    }

}
