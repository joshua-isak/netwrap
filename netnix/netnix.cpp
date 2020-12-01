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
            lasterror = z;
            return -1;
        }

        // Extract results and set them to &results
        memset(result, 0, sizeof(*result));
        memcpy(result, resinfo->ai_addr, resinfo->ai_addrlen);

        freeaddrinfo(resinfo);

        return 0;
    }


    // Send data to an address on the socket (UDP)
    int sendto(char data[], const char * address, const char * port);


    // Send data to an address on the socket (UDP using sockaddr_storage)
    int sendto(char data[], sockaddr_storage destination) {
        // Send buffer data
        int res = ::sendto(socket, data, strlen(data), 0, (sockaddr *) &destination, (socklen_t) destination.ss_len);
    
        // Check for errors
        if (res == -1) {
            lasterror = "Socket.sendto: error sending data";
            return -1;
        }

        return res;     // return number of bytes sent
    }


    // Read data from the incoming socket buffer
    int recv(char recvbuf[], int recvbuflen);

    // Read data and sender address from incoming socket buffer
    int recvfrom(char recvbuf[], int recvbuflen, sockaddr_storage &from);

    // Close the socket
    void close();

};