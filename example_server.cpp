#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include "TCPAcceptor.h"
#include <string>
#include <thread>


using namespace std;

void new_connection(TCPStream *stream){
    ssize_t len;
    char line[1000];
    while ((len = stream->receive(line, sizeof(line))) > 0)
    {
        line[len] = 0;
        printf("received - \n%s\n", line);
        string rec(line);
        // Check if the server is just trying to test TCP connection
        if(rec.compare("Ping") == 0)
            stream->send("Pong",5);
    }
    delete stream;
}

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        printf("Usage: server <port>\n");
        exit(1);
    }

    TCPStream* stream = NULL;
    TCPAcceptor* acceptor = NULL;
    
    acceptor = new TCPAcceptor(atoi(argv[1]));

    if (acceptor->start() == 0)
    {
        for(;;)
        {
            stream = acceptor->accept();
            if (stream != NULL)
            {
                std::thread t1(new_connection, stream);
                t1.detach();
            }
        }
    }
    exit(0);
}



