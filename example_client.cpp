#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include "TCPConnector.h"

using namespace std;

bool test_tcp_connection(TCPStream* stream);

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        printf("usage: %s <port> <ip>\n", argv[0]);
        exit(1);
    }

    string message;
    
    TCPConnector* connector = new TCPConnector();
    TCPStream* stream = connector->connect(argv[2],atoi(argv[1]));

    if(test_tcp_connection(stream))
        cout << "\n===================================\nTCP test passed \n===================================\n\n\n";

    exit(0);
}




bool test_tcp_connection(TCPStream* stream)
{
    // Test 1 : Test if we are able to establish a TCP connection
    if (stream)
    {
        int len;
        char line[7];
        string testPing = "Ping";
        cout << "Sent: Ping\n" << "\n";
        stream->send(testPing.c_str(),testPing.size());
        len = stream->receive(line,sizeof(line));
        string response(line);
        cout << "Got: " << response << "\n";
        for(;;){

        }
//        delete stream;
//        if(response.compare("Pong") == 0)
//            return true;
//        return false;

    }
}
