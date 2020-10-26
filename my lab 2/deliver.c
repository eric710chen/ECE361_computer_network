#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
//#include <netdb/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>

typedef struct Packet {
	unsigned int total_frag;
	unsigned int frag_no;
	unsigned int size;
	char* filename;
	char filedata[1000];
} Packet;

void main(int argc, char** argv) {

    //if argv is less than 3 arguments, not enoug inputs, exit
    if (argc < 3) {
        printf("No port number\n");
        exit(0);
    }
    int port = atoi(argv[2]);

    char *server_address = argv[1];
    int sockfd;
    char buffer[1024];
    struct sockaddr_in server_addr;
    socklen_t addr_size;

    sockfd = socket(PF_INET, SOCK_DGRAM, 0);
    memset(&server_addr, '\0', sizeof (server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(server_address);

    //printf("Input a ftp <filename>\n");





    char *file = malloc(1);
    int c;
    int i = 0;
    /* Read characters until found an EOF or newline character. */
    while ((c = getchar()) != '\n' && c != EOF) {
        file[i++] = c;
        file = realloc(file, i + 1); // Add space for another character to be read.
    }
    file[i] = '\0'; // Null terminate the string
    //printf("file name is: %s\n",file);
    char *filename;
    char *p = strtok(file, " ");
    //printf("p is %s",p);
    if (strcmp(p, "ftp") == 0) {

        while (p != NULL) {
            //printf("%s\n",p);
            filename = p;
            p = strtok(NULL, " ");
        }

    }

    //if file exists
    if (access(filename, F_OK) != -1) {
        strcpy(buffer, "ftp");
        //printf("file found %s\n",filename);
    } else {
        printf("file not found %s\n", filename);
        close(sockfd);
        exit(0);
    }

	clock_t start, end;
	start = clock();
	//beginning of round trip

    //printf("helloooo\n");
    sendto(sockfd, buffer, 1024, 0, (struct sockaddr*) &server_addr, sizeof (server_addr));
    printf("Data sent: %s\n", buffer);
    //recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr*)& si_other, &addr_size);
    addr_size = sizeof (server_addr);
    recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr*) &server_addr, &(addr_size));

	// end of round trip
	end = clock();
	double traveltime = (end - start) / CLOCKS_PER_SEC;
	printf("Round trip time is %f.\n", traveltime);

    if (strcmp(buffer, "yes") == 0)
        printf("A file transfer can start.\n");
    else {
        close(sockfd);
        exit(0);
    }

	memset(buffer, 0, sizeof(buffer));
	//clean buffer





	//find the number of packets needed
	FILE *f;
	int total_frag;
	f = fopen(filename, "r");
	fseek(f, 0, SEEK_END);
	total_frag = (ftell(f) / 1000) + 1;
	printf("Total number of fragments: %d.\n", total_frag);

	//divid file into packets
	//char buffer[1024];
	char **frag = malloc(sizeof(char *) * total_frag);
	for (int n = 0; n < total_frag; n++) {
		Packet packet;
		packet.total_frag = total_frag;
		packet.grag_no = n + 1;
		packet.filename = filename;
		if ((n + 1) == total_frag) {	//the last packet
			fseek(f, 0, SEEK_END);
			int frag_size = ftell(f) % 1000;
			packet.size = frag_size;
			memset(pack.filedata, 0, sizeof(char) * (1000));
			fread((void *)packet.filedata, sizeof(char), 1000, f);
		} else {						//all the full 1000 char packets
			packet.size = DATA_SIZE;
			memset(pack.filedata, 0, sizeof(char) * (1000));
			fread((void *)packet.filedata, sizeof(char), 1000, f);
		}
	}
	
	//send packet and receive acknowledgement
	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv.usec = 500000;
	// this is a timeout of 0.5 sec.

	setsockopt(sockfd, SOL_SOCKET, SO_REVTIMEO, (char *)&timeout, sizeof(timeout));
	socklen_t addr_size = sizeof (server_addr);
	
	for (int m = 0; m < total_frag; m++) {
		sendto(sockfd, packets[m], 1024, 0, (struct sockaddr*) &server_addr, sizeof(server_addr)));
		if (recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr*) &server_addr, &(addr_size)) == -1) {
			printf("recvfrom timed out at packet #%d.\n", m);
		}
		//now read the ack packet

	}







    //note that we do not need to bind the socket to address for client in UDP, it is beacuse of UDP
    free(file);
    close(sockfd);
}
