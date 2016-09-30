#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include "TCP/TCPAcceptor.h"
#include "Application/ClientQueue.h"
#include "Utilities.h"
#include <vector>
#include "Application/Channel.h"
#include <string>
#include <thread>


using namespace std;

void new_connection(TCPStream *stream, ClientQueue *c);
void new_channel(User* person1, User* person2);
void connectionManager(ClientQueue* c);


TCPAcceptor* acceptor = NULL;

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        printf("Usage: server <port>\n");
        exit(1);
    }

    ClientQueue cqueue = ClientQueue();
    acceptor = new TCPAcceptor(atoi(argv[1]));

    if (acceptor->start() == 0)
    {
        std::thread connectionThread(connectionManager, &cqueue);
        connectionThread.detach();

        for(;;)
        {
            if(cqueue.getSize() > 1){

                User* firstperson = cqueue.getNext();
                User* secondperson = cqueue.getNext();

                std::thread channelthread(new_channel,firstperson, secondperson);
                channelthread.detach();
            }

        }
    }
    exit(0);
}

void new_channel(User* person1, User* person2){

    Channel talk = Channel();
    talk.addUser(person1);
    talk.addUser(person2);

    send("Found someone! Connecting you to a chat.", person1->getUserStream());
    send("Found someone! Connecting you to a chat.", person2->getUserStream());

    ssize_t len;
    char line[1000];

    for(;;){
        if ((len = person1->getUserStream()->receive(line, sizeof(line))) > 0){
            line[len] = 0;
            printf("User1 sent - \n%s\n", line);
            string msg(line);
            std::vector<std::string> splitCommand=split(msg,' ');
            if(splitCommand[0].compare("TRANSFER")){
                talk.transferFile(person1,splitCommand[1],atol(splitCommand[0].c_str()));
            }
            //talk.sendMessage(person1,msg);
        }
        if ((len = person2->getUserStream()->receive(line, sizeof(line))) > 0){
            line[len] = 0;
            printf("User2 sent - \n%s\n", line);
            string msg(line);
            talk.sendMessage(person2,msg);
        }
    }

}

void new_connection(TCPStream *stream, ClientQueue *cqueue){
    ssize_t len;
    char line[1000];

    while ((len = stream->receive(line, sizeof(line))) > 0)
    {
        line[len]=0;
        std::ostringstream oss;
        printf("received - \n%s\n", line);
        printf(line);
        std::string rec(line);
        User me;

        vector<string> splitCommand = split(rec, ' ');
        if(rec.compare("Ping"))
        {stream->send("Pong",5);}
        if(splitCommand[0].compare("CONNECT") == 0) {
            me = User(splitCommand[1], stream,  false);
            cqueue->addUser(&me);

            int queuesize = cqueue->getSize();
            oss << "You are position" << queuesize << "in the queue.";
            send(oss.str(), stream);

            if(cqueue->getSize() <= 1) {
                oss << "Currently no one else is online. Sorry :(. We'll match you with someone"
                        "once there is another person!\n";
                send(oss.str(), stream);
            }

        }

    }
    delete stream;
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