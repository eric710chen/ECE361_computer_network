#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//argc = total number of inputs 
//argv the array of inputs
void main(int argc, char **argv) {

    //has to be 2 1-file name 2- port number
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        printf("No port provided\n");
        exit(0);
    }

    //convert the port number from argv to an integer
    int port = atoi(argv[1]);
    int sockfd;

    //sockaddr_in is a structure that specifies the addresses to which sockets are binded
    //in this case si_me specifies addresses of the server socker and si_other specifies addresses for the client socket
    struct sockaddr_in si_me, si_other;
    char buffer[1024];

    //size of addr structures si_me and si_other
    socklen_t addr_size;

    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////Create the socket
    //SOCK_DGRAM is the type of socket; used for UDP, while SOCK_STREAM is used for TCP
    //AF_INET is the domain for IPv4
    //0 is the default protocol
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    //fill the memory at address si_me 
    memset(&si_me, '\0', sizeof (si_me));

    //specify data in the struct for the socket 
    si_me.sin_family = AF_INET;

    //htons converts an IP port number in host to IP port number in network byte order 
    si_me.sin_port = htons(port);
    si_me.sin_addr.s_addr = inet_addr("127.0.0.1");


    /////bind the socket
    bind(sockfd, (struct sockaddr*) &si_me, sizeof (si_me));

    //receive a message/data from the socket
    addr_size = sizeof (si_other);

    //buffer is the application buffer in which the data is received
    //1024 is the buffer size
    //0 is Bitwise OR flags, 0 indicates no change to the socket behaviour
    //si_other is the struct for the client socket
    ////addr_size if the size of the client socket structure(si_other)
    recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr*) & si_other, &addr_size);


    printf("Data received: %s\n", buffer);

    if (strcmp(buffer, "ftp") == 0) {
        //buffer="yes";
        memset(buffer, 0, sizeof (buffer));
        strcpy(buffer, "yes");
        //response message
        sendto(sockfd, buffer, 1024, 0, (struct sockaddr*) & si_other, sizeof (si_other));
        close(sockfd);
        //printf("%s\n",buffer);
    }
    else {
        //printf("what it is: %s, what it should be: %s",buffer,"ftp");
        memset(buffer, 0, sizeof (buffer));
        strcpy(buffer, "no");
        printf("no\n");
        //response message
        //sendto(sockfd, buffer, 1024, 0, (struct sockaddr*)& si_other, sizeof(addr_size));
        printf("hi");

        close(sockfd);
        exit(0);
    }


}
