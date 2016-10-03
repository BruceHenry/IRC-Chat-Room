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
void new_channel(Channel* newch, User* person1, User* person2);
void remove_channel(Channel* ch);
void connectionManager(ClientQueue* c);
void adminServer(ClientQueue *cqueue, vector<Channel *> * channels);


TCPAcceptor* acceptor = NULL;
vector<Channel*> channels;
ClientQueue cqueue;

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        printf("Usage: server <port>\n");
        exit(1);
    }
    cqueue = ClientQueue();

    acceptor = new TCPAcceptor(atoi(argv[1]));
    channels = vector<Channel*>();

    if (acceptor->start() == 0)
    {
        std::thread connectionThread(connectionManager, &cqueue);
        connectionThread.detach();

        for(;;)
        {
            if(cqueue.getSize() > 1){

                User* firstperson = cqueue.getNext();
                User* secondperson = cqueue.getNext();

                Channel *newChannel = new Channel();

                channels.push_back(newChannel);

                new_channel(newChannel, firstperson, secondperson);

            }

        }
    }
    exit(0);
}

void new_channel(Channel *ch, User* person1, User* person2){

    ch->addUser(person1);
    ch->addUser(person2);

    person1->setActiveChannel(ch);
    person2->setActiveChannel(ch);

    send("IN_SESSION", person1->getUserStream());
    send("IN_SESSION", person2->getUserStream());

}

void new_connection(TCPStream *stream, ClientQueue *cqueue){
    ssize_t len;
    char line[1000];
    User me;

    while ((len = stream->receive(line, sizeof(line))) > 0)
    {
        line[len]=0;
        string rec(line);

        vector<string> splitCommand = split(rec, ' ');
        if(splitCommand[0].compare("CONNECT") == 0) {
            me = User(splitCommand[1], stream,  false);
            cqueue->addUser(&me);

            send("Welcome!", stream);

            if(cqueue->getSize() <= 1) {

                send("There is no one online at the moment :c", stream);
            }
            else{
                send("Type CHAT to connect to a random person!", stream);
            }
        }

        if(rec.compare("CHAT") == 0){
            if (&me != nullptr){
                me.setChatStatus(true);
            }
        }

        if(splitCommand[0].compare("MSG") == 0){
            string msg = rec;
            if (&me != nullptr)
                me.sendMessage(msg);
        }

        if(rec.compare("QUIT") == 0){
            if (&me != nullptr)
                me.sendMessage("QUIT " + me.getUsername());
            remove_channel(me.getActiveChannel());
        }

        if(rec.compare("HELP") == 0){
            string helpstring = "COMMANDS:\n HELP\n QUIT\n MSG <USERNAME> <MESSAGE>\n";
            send(helpstring, stream);
        }

    }
    delete stream;
}

void remove_channel(Channel* ch){
    std::vector<User*> users = ch->getUsers();
    for(User* u : users){
        u->leaveChannel();
        u->setChatStatus(false);
        cqueue.addUser(u);

    }
    delete ch;
}

void connectionManager(ClientQueue *c){
    for(;;) {
        TCPStream* stream = acceptor->accept();
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
            cout<<"To generate a log, please input: STATS\n";
            cout<<"To throwout someone, please input: THROWOUT username\n";
            cout<<"To block someone, please input: BLOCK username\n";
            cout<<"To unblock someone, please input: UNBLOCK username\n";
            cout<<"To end serving, please input: END\n";
            continue;
        }

        if (splitCommand[0].compare("STATS") == 0 && splitCommand.size() == 1) {
            const char *filename2 = "/tmp/server_log.txt";
            ofstream out(filename2, ios::app);
            string buffer="\n";
            time_t t;
            time (&t);
            buffer.append(ctime(&t));
            buffer.append("Queue Size: ");
            buffer.append(to_string(cqueue->getSize()));
            buffer.append("\nChannel Size: ");
            buffer.append(to_string(channels->size()));
            buffer.append("\nTotal user numbers:: ");
            buffer.append(to_string(usernumber));
            buffer.append("\n");
            cout<<buffer;
            out<<buffer;
            out.close();
            continue;
        }

        if (splitCommand[0].compare("THROWOUT") == 0 && splitCommand.size() == 2) {

        }

        if (splitCommand[0].compare("BLOCK") == 0 && splitCommand.size() == 2) {

        }

        if (splitCommand[0].compare("UNBLOCK") == 0 && splitCommand.size() == 2) {

        }

        if (splitCommand[0].compare("END") == 0 && splitCommand.size() == 1) {

        }
        cout<<"Invalid command! To get help, please input: HELP\n";
    }
