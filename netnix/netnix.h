// C UNIX socket stuff
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 

class Socket {
public:
    int socket;
    char * lasterror;
    struct sockaddr_storage recvaddr;

    // Initialize the socket
    int init(int type, int af, int protocol);

    // Resolve a network address into a sockaddr_storage
    int resolve(char * address, char * port);

    // Send data to an address on the socket (UDP)
    int sendto(char data[], const char * address, const char * port);

    // Send data to an address on the socket (UDP using sockaddr_storage)
    int sendto(char data[], sockaddr_storage destination);

    // Read data from the incoming socket buffer
    int recv(char recvbuf[], int recvbuflen);

    // Read data and sender address from incoming socket buffer
    int recvfrom(char recvbuf[], int recvbuflen, sockaddr_storage &from);

    // Close the socket
    void close();

};