//
// Created by Erik Nadel on 9/27/16.
//

#ifndef PROJECT1_CHANNEL_H
#define PROJECT1_CHANNEL_H


#include <string>
#include <vector>
#include "User.h"
#include "Message.h"

class Channel {
public:
    Channel();
    void addUser(User* u);
    void sendMessage(User* u, std::string message);
    void openChannel();
    void transferFile(User* u, std::string filename,long filesize);

private:
    std::vector<Message> messages;
    std::vector<User*> users;

};


#endif //PROJECT1_CHANNEL_H
