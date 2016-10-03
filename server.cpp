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

void adminServer(ClientQueue *cqueue, vector<Channel *> *channels);

void deleteChannel(Channel *c);

int userNumber(void);


TCPAcceptor *acceptor = NULL;
vector<Channel *> channels;
ClientQueue cqueue;

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: server <port>\n");
        exit(1);
    }
    cqueue = ClientQueue();

    acceptor = new TCPAcceptor(atoi(argv[1]));
    channels = vector<Channel *>();
    string cmd;
    while (1) {
        cout << "To start sever, please input: START\n";
        cin >> cmd;
        if (cmd.compare("START") == 0)
            break;
    }

    cout << "The server is running.\n";
    std::thread ad(adminServer, &cqueue, &channels);
    ad.detach();

    if (acceptor->start() == 0) {
        std::thread connectionThread(connectionManager, &cqueue);
        connectionThread.detach();

        for (;;) {
            if (cqueue.getSize() > 1) {

                User *firstperson = cqueue.getNext();
                User *secondperson = cqueue.getNext();

                Channel *newChannel = new Channel();

                channels.push_back(newChannel);

                new_channel(newChannel, firstperson, secondperson);

            }

        }
    }
    exit(0);
}

void new_channel(Channel *ch, User *person1, User *person2) {

    ch->addUser(person1);
    ch->addUser(person2);

    person1->setActiveChannel(ch);
    person2->setActiveChannel(ch);

    send("IN_SESSION", person1->getUserStream());
    send("IN_SESSION", person2->getUserStream());

}

void new_connection(TCPStream *stream, ClientQueue *cqueue) {
    ssize_t len;
    char line[1000];
    User me;

    while ((len = stream->receive(line, sizeof(line))) > 0) {
        line[len] = 0;
        string rec(line);
        bool pingFlag = false;


        vector<string> splitCommand = split(rec, ' ');
        if (splitCommand[0].compare("CONNECT") == 0) {
            me = User(splitCommand[1], stream, false);
            cqueue->addUser(&me);

            send("Welcome!", stream);

            if (cqueue->getSize() <= 1) {

                send("There is no one online at the moment :c", stream);
            } else {
                send("Type CHAT to connect to a random person!", stream);
            }
        }

        if (rec.compare("PING") == 0 && !pingFlag) {
            stream->send("PONG", 5);
            pingFlag = true;
            continue;
        }

        if (rec.compare("CHAT") == 0) {
            if (&me != nullptr) {
                me.setChatStatus(true);
            }
        }

        if (splitCommand[0].compare("MSG") == 0) {
            string msg = rec;
            if (&me != nullptr)
                me.sendMessage(msg);
        }

        if (rec.compare("QUIT") == 0) {
            //handling when 2 users are in a channel
            if (&me != nullptr)
                me.sendMessage("QUIT " + me.getUsername());
            deleteChannel(me.getActiveChannel());
            remove_channel(me.getActiveChannel());

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

        if (!me.getActiveChannel()) {
            string s = "You are not matched with someone!\n";
            send(s, stream);
            continue;
        }

        if (rec.compare("HELP") == 0) {
            string helpstring = "COMMANDS:\n HELP\n QUIT\n MSG <USERNAME> <MESSAGE>\n";
            send(helpstring, stream);
        }

    }
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

void adminServer(ClientQueue *cqueue, vector<Channel *> *channels) {
    string cmd;
    while (1) {
        cin.clear();
        std::getline(cin, cmd);
        if (cmd.compare("") == 0)
            continue;
        std::vector<std::string> splitCommand = split(cmd, ' ');

        if (splitCommand[0].compare("HELP") == 0) {
            cout << "To generate a log, please input: STATS\n";
            cout << "To throwout someone, please input: THROWOUT username\n";
            cout << "To block someone, please input: BLOCK username\n";
            cout << "To unblock someone, please input: UNBLOCK username\n";
            cout << "To end serving, please input: END\n";
            continue;
        }

        if (splitCommand[0].compare("STATS") == 0 && splitCommand.size() == 1) {
            const char *filename2 = "/tmp/server_log.txt";
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
            buffer.append("\n");
            cout << buffer;
            out << buffer;
            out.close();
            continue;
        }

        if (splitCommand[0].compare("THROWOUT") == 0 && splitCommand.size() == 2) {
            for (Channel *c:*channels) {
                for (User *u: c->getUsers()) {
                    if (splitCommand[1].compare(u->getUsername())==0) {
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

        }

        if (splitCommand[0].compare("UNBLOCK") == 0 && splitCommand.size() == 2) {

        }

        if (splitCommand[0].compare("END") == 0 && splitCommand.size() == 1) {

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

