//
// Created by Erik Nadel on 9/28/16.
//

#ifndef PROJECT1_MESSAGE_H
#define PROJECT1_MESSAGE_H


#include "User.h"

class Message {
public:
    Message();
    Message(User* sender, std::string message, int id);
    User* getUser();

private:
    User* sender;
    std::string msg;
    int id;
};


#endif //PROJECT1_MESSAGE_H
