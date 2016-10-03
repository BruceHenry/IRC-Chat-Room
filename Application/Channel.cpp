//
// Created by Erik Nadel on 9/27/16.
//

#include <cstdlib>
#include "Channel.h"
#include <iostream>

Channel::Channel() {
    users = std::vector<User*>();

}

void Channel::addUser(User* u) {
    this->users.push_back(u);
}

std::vector<User*> Channel::getUsers() {
    return this->users;
}


void Channel::sendMessage(User* sender, std::string message) {

    for(User* u : users){
        if(*u != *sender){
            TCPStream* s = u->getUserStream();
            s->send(message.c_str(),message.length());
        }
    }

}

void Channel::transferFile(User *sender, std::string filename, long filesize) {
    char *mfcc;
    cout << "Tranfering a file" << endl;
    long sizecheck = 0;
    int received = 0;

    TCPStream *s = sender->getUserStream();
    std::string filemessage = "FILE" + std::string(" ") + filename + std::string(" ") + std::to_string(filesize);
    int i = 0;
    while (sizecheck < filesize) {
        if ((filesize - sizecheck) <= 1500) {
            mfcc = new char[(filesize - sizecheck)];
            received = s->receive(mfcc, filesize - sizecheck);
        } else {
            mfcc = new char[1500];
            received = s->receive(mfcc, 1500);
        }
        i++;
        for (User *u:users) {
            if (*u != *sender) {
                TCPStream *r = u->getUserStream();
                if (i == 1) {
                    r->send(filemessage.c_str(), filemessage.size());
                    sleep(0.5);
                }
                r->send(mfcc, received);
            }
        }
        sizecheck += received;
        if (received == 0)
            break;
    }
    printf("Have finished file transfering\n");
}

User* Channel::getOtherUser(User* u){
    for (User *a:users){
        if (*a != *u)
            return a;
    }
}