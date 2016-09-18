#include <arpa/inet.h>
#include "TCPStream.h"


TCPStream::TCPStream()
{

}

TCPStream::TCPStream(int sd, struct sockaddr_in* address)
: streamSocket(sd)
{
    char ip[50];
    // Get the IP of this machine
    inet_ntop(PF_INET, (struct in_addr*)&(address->sin_addr.s_addr), ip, sizeof(ip)-1);
    peerIP = ip;
    peerPort = ntohs(address->sin_port);
}

ssize_t TCPStream::send(const char* buffer, size_t len)
{
    return write(streamSocket,buffer,len);
}

ssize_t TCPStream::receive(char* buffer, size_t len)
{
    return read(streamSocket, buffer, len);
}

string TCPStream::getPeerIP()
{
    return peerIP;
}

int TCPStream::getPeerPort()
{
    return peerPort;
}

TCPStream::~TCPStream()
{
    close(streamSocket);
}
