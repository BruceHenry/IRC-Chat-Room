#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>
#include "TCPConnector.h"

TCPStream* TCPConnector::connect(const char* server, int port)
{
    struct sockaddr_in address;

    memset (&address, 0 , sizeof(address));
    // Set the family to the Address Family
    address.sin_family = AF_INET;
    address.sin_port = htons(port);

    // If the host name is resolved into an ip, binarize the ip and put it in sockaddr
    if (resolveHostName(server, &(address.sin_addr)) != 0)
        inet_pton(PF_INET, server, &(address.sin_addr));

    int sd = socket(AF_INET, SOCK_STREAM, 0);

    if(::connect(sd, (struct sockaddr*)&address, sizeof(address)) != 0)
    {
        perror("Connect() failed");
        return NULL;
    }

    // Return the stream to the target server
    return new TCPStream(sd, &address);
}

int TCPConnector::resolveHostName(const char* hostname, struct in_addr* addr)
{
    struct addrinfo *res;

    int result = getaddrinfo (hostname, NULL, NULL, &res);
    if (result == 0)
    {
        // Move the address info into the in_addr struct that was passed in
        memcpy(addr, &((struct sockaddr_in *) res->ai_addr)->sin_addr,sizeof(struct in_addr));
        freeaddrinfo(res);
    }
    return result;
}
