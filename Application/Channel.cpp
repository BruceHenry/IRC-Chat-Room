//
// Created by Erik Nadel on 9/27/16.
//

#include <cstdlib>
#include "Channel.h"

Channel::Channel() {
    users = std::vector<User*>();

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

void Channel::transferFile(User* sender, std::string filename, long filesize) {
    char* mfcc;
    long sizecheck = 0;
    int received=0;
    TCPStream* s=sender->getUserStream();
    std::string filemessage="/file"+std::string(" ")+filename+std::string(" ")+std::to_string(filesize);
    if(filesize >1499){
        mfcc=(char *) malloc(1500);
        int i=1;
        while(sizecheck<filesize){
            received=s->receive(mfcc,1500);
            i++;
            for (User* u:users) {

                TCPStream* r=u->getUserStream();
                if(i==0) {
                    r->send(filemessage.c_str(), filemessage.size());
                }
                r->send(mfcc,received);

            }
            sizecheck+=received;

        }
    }
    else{
        mfcc=(char *) malloc(filesize+1);
        received=s->receive(mfcc,filesize);
        for (User* u:users) {
            TCPStream* r=u->getUserStream();
            r->send(filemessage.c_str(), filemessage.size());
            r->send(mfcc,received);

        }

    }

}
