//
// Created by Erik Nadel on 9/28/16.
//

#include "Message.h"

Message::Message(User* sender, std::string message, int id) {
    this->sender = sender;
    msg = message;
    this->id = id;

}

User* Message::getUser() {
    return this->sender;
}
