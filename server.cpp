#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include "TCP/TCPAcceptor.h"
#include "Application/ClientQueue.h"
#include "Utilities.h"
#include <vector>
#include <string>
#include <thread>


using namespace std;

void new_connection(TCPStream *stream, ClientQueue *c);

void new_channel(Channel *newch, User *person1, User *person2);

void remove_channel(Channel *ch);

void connectionManager(ClientQueue *c);

void admin(ClientQueue *cqueue, vector<Channel *> *channels);

void deleteChannel(Channel *c);

void cqueueThread();

bool checkChatStatus(User **firstUser, User **secendUser);

int userNumber(void);


TCPAcceptor *acceptor = NULL;
vector<Channel *> channels;
ClientQueue cqueue;
int clientCount = 0;
int maxUsers = 8;

int main(int argc, char **argv) {
    cqueue = ClientQueue();
    int serverport = 5555;


    acceptor = new TCPAcceptor(serverport);
    channels = vector<Channel *>();
    string cmd;


    while (1) {
        cout << "To start server, please input: START\n";
        cin >> cmd;
        if (cmd.compare("START") != 0)
            continue;
        cout << "The server is running.\n";
        std::thread ct(cqueueThread);
        std::thread connectionThread(connectionManager, &cqueue);
        ct.detach();
        if (acceptor->start() == 0)
            connectionThread.detach();
        else {
            cout << "TCP initialization failed!\n";
            continue;
        }
        admin(&cqueue, &channels);
    }
}

void cqueueThread() {
    for (;;) {
        if (cqueue.getSize() > 1) {
            User *firstperson;
            User *secondperson;

            if (checkChatStatus(&firstperson, &secondperson) == false)
                continue;

            Channel *newChannel = new Channel();

            channels.push_back(newChannel);

            new_channel(newChannel, firstperson, secondperson);
        }
    }
}

void new_channel(Channel *ch, User *person1, User *person2) {

    ch->addUser(person1);
    ch->addUser(person2);

    person1->setActiveChannel(ch);
    person2->setActiveChannel(ch);

    string person1greeting = "IN_SESSION with " + person2->getUsername();
    string person2greeting = "IN_SESSION with " + person1->getUsername();

    send(person1greeting,  person1->getUserStream());
    send(person2greeting, person2->getUserStream());

}

void new_connection(TCPStream *stream, ClientQueue *cqueue) {
    ssize_t len;
    char line[1000];
    User me;

    while ((len = stream->receive(line, sizeof(line))) > 0) {
        line[len] = 0;
        string rec(line);

        if (rec.compare("PING") == 0) {
            stream->send("PONG", 5);
            continue;
        }

        if (rec.compare("HELP") == 0) {
            send("COMMANDS:\n CHAT : To be ready to chat\n", stream);
            send("MSG <content> : To send messages while chatting with others\n", stream);
            send("FIle <filename> <filepath> : To send a file(<100 MB)\n", stream);
            send("QUIT : To quit chatting and be in the queue\n", stream);
            send("EXIT : To exit the app\n", stream);
            send("HELP : To get help\n", stream);
            continue;
        }

        vector<string> splitCommand = split(rec, ' ');
        if (splitCommand[0].compare("CONNECT") == 0) {

            if(userNumber() >= maxUsers){
                send("Sorry. The chat server is full :(. Come back another time!", stream);
                break;
            }

            me = User(splitCommand[1], stream, clientCount);
            clientCount++;
            cqueue->addUser(&me);
            send("Welcome!\n", stream);
            send("Please input CHAT when you are ready to chat.\n", stream);
            continue;
        }

        if (rec.compare("CHAT") == 0) {
            if (me.getBlockFlag() == true) {
                send("You are blocked by the admin!\n", stream);
                continue;
            }
            else if (&me != nullptr) {
                me.setChatStatus(true);
            }
            send("We are finding someone for you.\n", stream);
            continue;
        }

        if (!me.getActiveChannel()) {
            if (me.getBlockFlag() == true) {
                send("You are blocked by the admin!\n", stream);
                continue;
            }
            send("You are not matched with someone!\n", stream);
            if (me.getChatStatus() == false)
                send("Please input CHAT when you are ready to chat.\n", stream);
            continue;
        }

        if (splitCommand[0].compare("FILE") == 0) {
            me.getActiveChannel()->transferFile(&me, splitCommand[1], stol(splitCommand[2]));
            const char *filename2 = "/tmp/server_log.txt";
            ofstream out(filename2, ios::app);
            string buffer = "\n";
            time_t t;
            time(&t);
            buffer.append(ctime(&t));
            buffer.append("From User: ");
            buffer.append(me.getUsername());
            buffer.append(" to User: ");
            buffer.append(me.getActiveChannel()->getOtherUser(&me)->getUsername());
            buffer.append("\nFile name: ");
            buffer.append(splitCommand[1]);
            buffer.append("  File size: ");
            buffer.append(splitCommand[2]);
            buffer.append("\n");
            out << buffer;
            out.close();
            continue;
        }

        if (splitCommand[0].compare("MSG") == 0) {
            string msg = rec;
            if (&me != nullptr)
                me.sendMessage(msg);
            continue;
        }

        if (rec.compare("QUIT") == 0) {
            //handling when 2 users are in a channel
            if (&me != nullptr)
                me.sendMessage("QUIT " + me.getUsername());
            deleteChannel(me.getActiveChannel());
            remove_channel(me.getActiveChannel());
            continue;
        }

        if (splitCommand[0].compare("EXIT") == 0) {
            if (!me.getActiveChannel()) {//while in the queue
                send(splitCommand[0], stream);
                break;
            }
            if (me.getActiveChannel() != NULL) {//Quit while chatting with someone
                string msg = "The other client quited, now you are added to the queue!\n";
                me.sendMessage(msg);
                cqueue->addUser(me.getActiveChannel()->getOtherUser(&me));
                deleteChannel(me.getActiveChannel());
                me.getActiveChannel()->getOtherUser(&me)->setActiveChannel(NULL);
                delete me.getActiveChannel();
            }
            send(splitCommand[0], stream);
            cout << "User: " << me.getUsername() << " quited!\n";
            cout << "usernumber:" << userNumber() << endl;
            break;
        }
    }

    if (!me.getActiveChannel()) {//while in the queue
        send("EXIT", stream);
    }
    if (me.getActiveChannel() != NULL) {//Quit while chatting with someone
        string msg = "The other client quited, now you are added to the queue!\n";
        me.sendMessage(msg);
        cqueue->addUser(me.getActiveChannel()->getOtherUser(&me));
        deleteChannel(me.getActiveChannel());
        me.getActiveChannel()->getOtherUser(&me)->setActiveChannel(NULL);
        delete me.getActiveChannel();
    }
    cout << "User: " << me.getUsername() << " disconnected!\n";
    cout << "usernumber:" << userNumber() << endl;
    delete stream;
}

void remove_channel(Channel *ch) {
    std::vector<User *> users = ch->getUsers();
    for (User *u : users) {
        u->leaveChannel();
        u->setChatStatus(false);
        cqueue.addUser(u);
    }
    delete ch;
}

void connectionManager(ClientQueue *c) {
    for (;;) {
        TCPStream *stream = acceptor->accept();
        if (stream != NULL) {
            std::thread t1(new_connection, stream, c);
            t1.detach();
        }
    }
}

void admin(ClientQueue *cqueue, vector<Channel *> *channels) {
    string cmd;
    int n = 0;
    while (1) {
        cin.clear();
        std::getline(cin, cmd);
        if (cmd.compare("") == 0)
            continue;
        std::vector<std::string> splitCommand = split(cmd, ' ');

        if (splitCommand[0].compare("HELP") == 0) {
            cout << "COMMANDS:\n";
            cout << "STATS : To generate a log\n";
            cout << "THROWOUT <username> : To throwout someone\n";
            cout << "BLOCK <username> : To block a user\n";
            cout << "UNBLOCK <username> : To unblock a user\n";
            cout << "END : To end serving\n";
            continue;
        }

        if (splitCommand[0].compare("STATS") == 0 && splitCommand.size() == 1) {
            const char *filename2 = "/tmp/server_log.txt";
            int channelCount = 0;
            ofstream out(filename2, ios::app);
            string buffer = "\n";
            time_t t;
            time(&t);
            buffer.append(ctime(&t));
            buffer.append("Queue Size: ");
            buffer.append(to_string(cqueue->getSize()));
            buffer.append("\nChannel Size: ");
            buffer.append(to_string(channels->size()));
            buffer.append("\nTotal user numbers:: ");
            buffer.append(to_string(userNumber()));
            buffer.append("\nChannel Usage:\n");
            for (Channel *c:*channels){
                channelCount++;

                buffer.append(to_string(channelCount));
                buffer.append(": ");
                buffer.append(to_string(c->getDataTransferred()));
                buffer.append(" Bytes \n");
            }
            buffer.append("\n");
            cout << buffer;
            out << buffer;
            out.close();
            continue;
        }

        if (splitCommand[0].compare("THROWOUT") == 0 && splitCommand.size() == 2) {
            for (Channel *c:*channels) {
                for (User *u: c->getUsers()) {
                    if (splitCommand[1].compare(u->getUsername()) == 0) {
                        string msg = "The other client was thrown out by the admin, now you are added to the queue!\n";
                        u->sendMessage(msg);
                        msg = "You were thrown out and added to the queue!\n";
                        c->getOtherUser(u)->sendMessage(msg);
                        remove_channel(c);
                        deleteChannel(c);
                    }
                }
            }
            continue;
        }

        if (splitCommand[0].compare("BLOCK") == 0 && splitCommand.size() == 2) {
            for (Channel *c:*channels) {
                for (User *u: c->getUsers()) {
                    if (splitCommand[1].compare(u->getUsername()) == 0) {
                        string msg = "The other client was blocked by the admin, now you are added to the queue!\n";
                        u->sendMessage(msg);
                        msg = "You were blocked and added to the queue!\n";
                        c->getOtherUser(u)->sendMessage(msg);
                        u->blockUser();
                        remove_channel(c);
                        deleteChannel(c);
                    }
                }
            }
            continue;
        }

        if (splitCommand[0].compare("UNBLOCK") == 0 && splitCommand.size() == 2) {
            for (Channel *c:*channels) {
                for (User *u: c->getUsers()) {
                    if (splitCommand[1].compare(u->getUsername()) == 0) {
                        string msg = "You were now unblocked\n";
                        c->getOtherUser(u)->sendMessage(msg);
                        u->unblockUser();
                    }
                }
            }
        }


        if (splitCommand[0].compare("END") == 0 && splitCommand.size() == 1) {
            cout << "The server is shutting down.";
            for (Channel *c:*channels) {
                c->getUsers()[0]->sendMessage("The server is shut down!\n");
                c->getOtherUser(c->getUsers()[0])->sendMessage("EXIT");
            }
            n = cqueue->getSize();
            for (int i = 0; i < n; i++) {
                cqueue->getNext()->getUserStream()->send("EXIT", 5);
            }
            exit(0);
        }

        cout << "Invalid command! To get help, please input: HELP\n";
    }
}

void deleteChannel(Channel *c) {
    for (int i = 0; i < 2; i++) {
        if (channels[i]) {
            if (channels[i] == c) {
                channels.erase(channels.begin() + i);
            }
        }
    }
}

int userNumber() {
    return channels.size() * 2 + cqueue.getSize();
}


bool checkChatStatus(User **firstUser, User **secendUser) {
    User *u;
    bool findFirstFlag = false;
    int n = cqueue.getSize();
    for (int i = 0; i < n; i++) {
        u = cqueue.getNext();
        if (u->getChatStatus() && (!u->getBlockFlag())) {
            if (findFirstFlag == false) {
                *firstUser = u;
                findFirstFlag = true;
                continue;
            } else {
                *secendUser = u;
                return true;
            }
        }
        cqueue.addUser(u);
    }
    if (findFirstFlag == true) {
        cqueue.addUser(*firstUser);
    }
    return false;
}