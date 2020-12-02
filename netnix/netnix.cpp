// C UNIX socket stuff
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

//#include "netnix.h"

#include <iostream>

using namespace std;

class Socket {
public:
    int socket;
    int socktype;
    int af_family;
    const char * lasterror;
    struct sockaddr_storage recvaddr;


    // Initialize the socket
    int init(int af, int type, int protocol) {
        // Set some variables
        socktype = type;
        af_family = af;

        // Create the socket
        socket = ::socket(af, type, protocol);

        // Check for errors in socket creation
        if (socket < 0) {
            lasterror = "Socket.init: socket creation failed";
            return -1;
        }

        return 0;
    }


    // Resolve a network address into a sockaddr_storage
    int resolve(const char * address, const char * port, sockaddr_storage *result) {
        // Set hints
        addrinfo hints;
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = af_family;
        hints.ai_socktype = socktype;

        // Put address results into resinfo
        addrinfo * resinfo;
        int err = getaddrinfo(address, port, &hints, &resinfo);

        // Check for errors
        if (err != 0) {
            // Write the error to lasterror
            const char * x = "Socket.resolve: ";
            const char * y = gai_strerror(err);
            char z[strlen(x) + strlen(y)];
            strcpy(z,x);
            strcpy(z,y);
            lasterror = "Socket.resolve: unable to resolve address/port";//z;
            return -1;
        }

        // Extract results and set them to &results
        memset(result, 0, sizeof(*result));
        memcpy(result, resinfo->ai_addr, resinfo->ai_addrlen);

        freeaddrinfo(resinfo);

        return 0;
    }


    // Send data to an address on the socket (UDP) using char * address data
    int sendto(char data[], const char * address, const char * port) {
        // Resolve destination address
        sockaddr_storage destination;
        int err = resolve((char *)address, (char *)port, &destination);
        if (err != 0) {  // Check for errors
            return -1;
        }

        // Send buffer data
        int res = ::sendto(socket, data, strlen(data), 0, (sockaddr *) &destination, (socklen_t) sizeof(destination));

        // Check for errors
        if (res == -1) {
            lasterror = "Socket.sendto: error sending data";
            return -1;
        }

        return res;     // return the number of bytes sent
    }


    // Send data to an address on the socket (UDP using sockaddr_storage)
    int sendto(char data[], sockaddr_storage destination) {
        // Send buffer data
        int res = ::sendto(socket, data, strlen(data), 0, (sockaddr *) &destination, (socklen_t) sizeof(destination));

        // Check for errors
        if (res == -1) {
            lasterror = "Socket.sendto: error sending data";
            return -1;
        }

        return res;     // return the number of bytes sent
    }


    // Read data from the incoming socket buffer
    int recv(char recvbuf[], int recvbuflen) {
        // Receive data on the socket
        int res = ::recv(socket, recvbuf, recvbuflen, 0);

        // Check for errors
        if (res < 0) {
            lasterror = "Socket.recv: error reading from socket";
            return -1;
        }

        return res;     // return the number of bytes received
    }


    // Read data and sender address from incoming socket buffer
    int recvfrom(char recvbuf[], int recvbuflen, sockaddr_storage &from) {
        int fromsize = sizeof(from);

        // Receive data on the socket
        int res = ::recvfrom(socket, recvbuf, recvbuflen, 0, (sockaddr *) &from, (socklen_t *) &fromsize);

        // Check for errors
        if (res < 0) {
            lasterror = "Socket.recvfrom: error reading from socket";
            return -1;
        }

        return res;     // return the number of bytes received
    }

    // Close the socket
    void close();

};