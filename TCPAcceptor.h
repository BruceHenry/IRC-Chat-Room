#ifndef TCPACCEPTOR_H
#define TCPACCEPTOR_H


#include <string>
#include <netinet/in.h>
#include "TCPStream.h"

using namespace std;

class TCPAcceptor
{
    int acceptorSocket;
    int portNum;
    bool isListening;

    public:
        TCPAcceptor(int port);
        ~TCPAcceptor();

        int start();
        TCPStream* accept();
    private:
        TCPAcceptor();
};

#endif // TCPACCEPTOR_H
