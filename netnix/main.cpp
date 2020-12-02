#include <stdio.h>
#include <string.h>

#include "netnix.cpp"

using namespace std;

int main(int argc, char *argv[]) {
    // Command line variables
    const char * ADDRESS = argv[1];
    const char * PORT = argv[2];
    char MESSAGE[] = "~hello world, from unix!";

    // Initialize a UDP socket
    Socket sock;
    int res = sock.init(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (res < 0) {  // Check for errors
        printf("Error: %s\n", sock.lasterror);
        return 0;
    }

    // Resolve destination address
    sockaddr_storage destination;
    res = sock.resolve((char *)ADDRESS, (char *)PORT, &destination);
    if (res < 0) {  // Check for errors
        printf("Error: %s\n", sock.lasterror);
        return 0;
    }

    // Send message to desination
    //res = sock.sendto(MESSAGE, destination);
    res = sock.sendto(MESSAGE, ADDRESS, PORT);
    if (res < 0) {  // Check for errors
        printf("Error: %s\n", sock.lasterror);
        return 0;
    }

    // Read received data from socket
    char recvdata[512];         // structure to store data read from socket (up to 512 bytes)
    sockaddr_storage sendaddr;  // structure to hold the sender address of the received packet
    res = sock.recvfrom(recvdata, sizeof(recvdata), sendaddr);
    if (res < 0) {  // Check for errors
        printf("Error: %s\n", sock.lasterror);
        return 0;
    }

    // Print the data!
    printf("Reply from server: %s\n", recvdata);

    // If we got here there were no errors woohoo!
    printf("No errors looks like it all worked!\n");
}