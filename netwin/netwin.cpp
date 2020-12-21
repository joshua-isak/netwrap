#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <stdio.h>
#include <string.h>

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

///// IN ORDER TO BUILD
// RUN: g++ -g test.cpp -lwsock32 -lWs2_32 -o test.exe   // -lwsock32 links the winsock dll

class Socket {
public:
    SOCKET socket = INVALID_SOCKET;
    int socktype;
    int af_family;
    const char * lasterror;

    struct addrinfo *result = NULL, *ptr = NULL, hints;
    int recvbuflen = 512;


    // Initialize the socket
    int init(int type, int af, int protocol) {
        // Initialize Winsock
        WSADATA wsaData;

        // Check for errors
        int res = WSAStartup(MAKEWORD(2,2), &wsaData);
        if (res != 0) {
            lasterror = "Socket.init: WSAStartup failed";
            return -1;
        }

        // Set some variables
        socktype = type;
        af_family = af;

        // Create the spcket
        socket = ::socket(af, type, protocol);

        // Check for errors in socket creation
        if (socket == INVALID_SOCKET) {
            lasterror = "Socket.init: socket creation failed";
            WSACleanup();
            return -1;
        }

        return 0;

        // // Initialize UDP or TCP socket parameters
        // if (type == SOCK_DGRAM) {
        //     hints.ai_family = af;
        //     hints.ai_socktype = SOCK_DGRAM;             //for tcp: SOCK_STREAM;
        //     hints.ai_protocol = IPPROTO_UDP;            //for tcp: IPPROTO_TCP
        //     protocol = IPPROTO_UDP;
        // } else if (type == SOCK_STREAM) {
        //     /* code */
        // }

        // Create the socket
        // sock = socket(af, type, protocol);

        // if (sock == INVALID_SOCKET) {      // Check for errors in creating the SOCKET
        //     printf("Networker: Error at socket(): %ld\n", WSAGetLastError());
        //     freeaddrinfo(result);
        //     WSACleanup();
        //     return -1;
        // }
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
            lasterror = "Socket.resolve: unable to resolve address/port";//z;
            return -1;
        }

        // Extract results and set them to &results
        memset(result, 0, sizeof(*result));
        memcpy(result, resinfo->ai_addr, resinfo->ai_addrlen);

        freeaddrinfo(resinfo);

        return 0;
    }


    // Send data to an address on the socket (UDP)
    int sendto(char data[], const char *address, const char *port) {

        // Resolve the server address and port
        int res = getaddrinfo(address, port, &hints, &result);
        if (res != 0) {
            lasterror = "Networker: getaddrinfo failed";
            return -1;
        }
        ptr=result;

        // Send a buffer
        res = ::sendto(socket, data, (int) strlen(data), 0, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (res == SOCKET_ERROR) {
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


    // Receive data on the socket
    int recv(char recvbuf[]) {

        // Receive data on the socket
        int iResult = ::recv(socket, recvbuf, recvbuflen, 0);
        if (iResult == 0)
            printf("Networker: Connection closed\n");   // returns 0
        else if (iResult < 0)
            printf("Networker: recv failed: %d\n", WSAGetLastError());  // returns -1

        return iResult;
    }


    // Receive data on the socket and report the source address
    int recvfrom(char recvbuf[], sockaddr_storage &from) {

        struct sockaddr_in sendaddr;
        int fromsize = sizeof(from);

        // Receive data on the socket
        int iResult = ::recvfrom(socket, recvbuf, recvbuflen, 0, (SOCKADDR *) &from, &fromsize);     //:: to specify not a recursive call
        if (iResult == 0)
            printf("Networker: Connection closed\n");   // returns 0
        else if (iResult < 0)
            printf("Networker: recv failed: %d\n", WSAGetLastError());  // returns -1

        return iResult;
    }


    // Close the socket
    void close() {
        closesocket(socket);
        WSACleanup();
    }

};






int main(int argc, char *argv[]) {

    // Get command line arguments
    char *ADDRESS = argv[1];
    char *PORT = argv[2];
    char data[] = "~hello world!";

    // Declare and initialize socket
    Socket sock;
    sock.init(SOCK_DGRAM, AF_UNSPEC, IPPROTO_UDP);

    // Send data on the socket
    sock.sendto(data, ADDRESS, PORT);

    // Recieve and print data and sender address from socket
    sockaddr_storage senderaddr;
    char result[512];
    int len = sock.recvfrom(result, senderaddr);

    result[len] = '\0';         // Make this char array printable...
    printf("Reply: %s\n", result);

    // Record and print sender data!
    socklen_t client_len = sizeof(struct sockaddr_storage);
    char hoststr[NI_MAXHOST];
    char portstr[NI_MAXSERV];

    int rc = getnameinfo((struct sockaddr *)&senderaddr, client_len, hoststr, sizeof(hoststr), portstr, sizeof(portstr), NI_NUMERICHOST | NI_NUMERICSERV);
    //if (rc == 0) printf("Address:  %s:%s\n", hoststr, portstr);
    printf("Address:  %s:%s\n", hoststr, portstr);

    // Send the data back!
    //sock.sendto(data, inet_ntoa(senderaddr.), PORT);

    // Close socket
    sock.close();

}