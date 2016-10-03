//
// Created by Erik Nadel on 9/27/16.
//

#include <cstdlib>
#include "ClientQueue.h"

ClientQueue::ClientQueue() {
    this->users = std::vector<User*>();
}

void ClientQueue::addUser(User *u) {
    this->users.push_back(u);

}

User* ClientQueue::getNext() {
    User *next;
    int randomIndex = rand() % this->users.size();
    next = this->users.at(randomIndex);
    this->users.erase(this->users.begin() + randomIndex);
    return next;
}

int ClientQueue::getChattyUsers() {
    int counter = 0;
    for(User* u: this->users){
        if(u->getChatStatus())
            counter++;
    }
    return counter;
}

int ClientQueue::getSize(){
    return this->users.size();
}


