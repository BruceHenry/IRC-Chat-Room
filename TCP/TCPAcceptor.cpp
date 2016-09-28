#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>
#include "TCPAcceptor.h"

TCPAcceptor::TCPAcceptor(int port)
    : acceptorSocket(0), portNum(port), isListening(false) {}

TCPAcceptor::~TCPAcceptor()
{
    if (acceptorSocket > 0)
        close(acceptorSocket);
}

int TCPAcceptor::start()
{
    if (isListening == true) // Prevent any errors
        return 0;

    // PF is protocol family
    acceptorSocket = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in address;

    memset(&address, 0, sizeof(address));
    address.sin_family = PF_INET; // Set the address family
    address.sin_port = htons(portNum); // Set the port number
    address.sin_addr.s_addr = INADDR_ANY; // Listen for any ip

    int optval = 1;

    // Allow other sockets to bind on this port. Preventing address already in use errors
    setsockopt(acceptorSocket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);

    // Attempt to bind our socket to an address
    int result = bind(acceptorSocket, (struct sockaddr*)&address, sizeof(address));
    if (result != 0){
        perror("bind() failed");
        return result;
    }

    int maxPendingConnections = 5;
    result = listen(acceptorSocket, maxPendingConnections);
    if (result != 0)
    {
        perror("listen() failed");
        return result;
    }

    isListening = true;
    return result;
}

// Extract first connection request in the queue
TCPStream* TCPAcceptor::accept()
{
    if (isListening == false)
        return NULL;

    struct sockaddr_in address;
    socklen_t len = sizeof(address);
    memset(&address, 0, sizeof(address));
    // attempt to take in next request
    int sd = ::accept(acceptorSocket, (struct sockaddr*)&address, &len);
    if (sd < 0)
    {
        perror("accept() failed");
        return NULL;
    }
    // Return the TCPStream to this connection
    return new TCPStream(sd, &address);
}
