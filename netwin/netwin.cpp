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
    SOCKET sock = INVALID_SOCKET;
    struct addrinfo *result = NULL, *ptr = NULL, hints;
    int recvbuflen = 512;


    // Socket(int protocol, int socktype) {

    // }

    // Initialize the socket
    int init(int type, int af) {

        int protocol;
        memset(&hints, 0, sizeof(hints));

        // Initialize UDP or TCP socket parameters
        if (type == SOCK_DGRAM) {
            hints.ai_family = af;
            hints.ai_socktype = SOCK_DGRAM;             //for tcp: SOCK_STREAM;
            hints.ai_protocol = IPPROTO_UDP;            //for tcp: IPPROTO_TCP
            protocol = IPPROTO_UDP;
        } else if (type == SOCK_STREAM) {
            /* code */
        }

        // Create the socket
        sock = socket(af, type, protocol);

        if (sock == INVALID_SOCKET) {      // Check for errors in creating the SOCKET
            printf("Networker: Error at socket(): %ld\n", WSAGetLastError());
            freeaddrinfo(result);
            WSACleanup();
            return -1;
        }
    }


    // Resolve an address and port
    int resolve();


    //#TODO change sendto so that it takes in real address data not just strings and looks them up every time....
    // Send data to an address on the socket (UDP)
    int sendto(char data[], const char *address, const char *port) {

        // Resolve the server address and port
        int iResult = getaddrinfo(address, port, &hints, &result);
        if (iResult != 0) {
            printf("Networker: getaddrinfo failed: %d\n", iResult);
            WSACleanup();
            return -1;
        }
        ptr=result;

        // Send a buffer
        iResult = ::sendto(sock, data, (int) strlen(data), 0, ptr->ai_addr, (int)ptr->ai_addrlen);  //:: to specify not a recursive call
        if (iResult == SOCKET_ERROR) {
            printf("Networker: send failed: %d\n", WSAGetLastError());
            closesocket(sock);
            WSACleanup();
            return -1;
        }

        return iResult;     // Return the number of bytes sent
    }


    // Receive data on the socket
    int recv(char recvbuf[]) {

        // Receive data on the socket
        int iResult = ::recv(sock, recvbuf, recvbuflen, 0);     //:: to specify not a recursive call
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
        int iResult = ::recvfrom(sock, recvbuf, recvbuflen, 0, (SOCKADDR *) &from, &fromsize);     //:: to specify not a recursive call
        if (iResult == 0)
            printf("Networker: Connection closed\n");   // returns 0
        else if (iResult < 0)
            printf("Networker: recv failed: %d\n", WSAGetLastError());  // returns -1

        return iResult;
    }


    // Close the socket
    void close() {
        closesocket(sock);
    }

};




// // return an IP address and port as a char * in the format "addr:port"
//     char * addrtos(sockaddr_in addr) {
//     }

//     // return the IP address as a char * (now supports IPV6!)
//     void iptos(sockaddr_storage addr, char * str) {
//         socklen_t client_len = sizeof(struct sockaddr_storage);
//         char hoststr[NI_MAXHOST];
//         char portstr[NI_MAXSERV];

//         int rc = getnameinfo((struct sockaddr *)&addr, client_len, hoststr, sizeof(hoststr), portstr, sizeof(portstr), NI_NUMERICHOST | NI_NUMERICSERV);

//         *str = *hoststr;
//     }

//     // return the port as a char *
//     void porttos(sockaddr_storage addr, char * str) {
//         socklen_t client_len = sizeof(struct sockaddr_storage);
//         char hoststr[NI_MAXHOST];
//         char portstr[NI_MAXSERV];

//         int rc = getnameinfo((struct sockaddr *)&addr, client_len, hoststr, sizeof(hoststr), portstr, sizeof(portstr), NI_NUMERICHOST | NI_NUMERICSERV);

//         str = portstr;
//     }




int main(int argc, char *argv[]) {

    // Get command line arguments
    char *ADDRESS = argv[1];
    char *PORT = argv[2];
    char data[] = "~hello world!";

    // Initialize Winsock
    WSADATA wsaData;

    int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        return 1;
    }

    // Declare and initialize socket
    Socket sock;
    sock.init(SOCK_DGRAM, AF_UNSPEC);


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

    // char hstr[NI_MAXHOST]; char pstr[NI_MAXSERV];
    // iptos(senderaddr, hstr);
    // porttos(senderaddr, pstr);
    // printf("Address2: %s:%s", hstr, pstr);

    // Send the data back!
    //sock.sendto(data, inet_ntoa(senderaddr.), PORT);


    // Close socket
    sock.close();

    // Cleanup Winsock
    WSACleanup();

}