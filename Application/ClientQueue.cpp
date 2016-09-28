//
// Created by Erik Nadel on 9/27/16.
//

#include "ClientQueue.h"

ClientQueue::ClientQueue() {
    this->users = std::queue<User*>();
}

void ClientQueue::addUser(User *u) {
    this->users.push(u);

}

User* ClientQueue::getNext() {
    User *next = this->users.front();
    this->users.pop();
    return next;
}

int ClientQueue::getSize(){
    return this->users.size();
}
