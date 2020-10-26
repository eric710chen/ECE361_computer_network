#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

int main(int argc, char const *argv[]) {
	
	if (argc != 2) {
		printf("invalid argv, format: server <server port num>\n");
		exit(0);
	}

	int port = atoi(argv[1]);	
	int buffSize = 256;
	char buff[buffSize] = {};
	struct sockaddr_in serverAddr, clientAddr;
	int sockfd = socket(PF_INET, SOCK_DGRAM, 0);

	if (sockfd < 0) {
		printf("Error: Socket open failed\n");
		exit(1);
	}
	
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port); //port num we got from above
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	int addrLen = sizeof(serverAddr.sin_zero);
	memset(serverAddr.sin_zero, 0, addrLen);
	
	// socket bind
	addrLen = sizeof(serverAddr);
	int bindSocket = bind(sockfd, (struct sockaddr *) &serverAddr, addrLen);
	if (bindSocket == -1) {
		printf("Error: socket bind failed.\n");
		exit(1);
	}

	// receive from client 
	addrLen = sizeof(clientAddr);
	socklen_t clientLength = addrLen;
	int receive = recvfrom(sockfd, buff, buffSize, 0, (struct sockaddr *) &clientAddr, &clientLength);
	if (receive == -1) {
		printf("Error, receive failed.\n");
		exit(1);
	}

	// send to client
	addrLen = sizeof(serverAddr);
	if (!strcmp(buff, "ftp", 4)) {
		int sendYes = sendto(sockfd, "yes", strlen("yes"), 0, (struct sockaddr *) &clientAddr, addrLen);
		if (sendYes == -1) {
			printf("Error: send message to cliend failed.\n");
			exit(1);
		} else {
			printf("Send message YES back to cliend Successful.\n");
		}
	} else {
		int sendNo = sendto(sockfd, "no", strlen("no"), 0, (struct sockaddr *) &clientAddr, addrLen);
		if (sendNo == -1) {
			printf("Error: send message to cliend failed.\n");
			exit(1);
		} else {
			printf("Send message NO back to cliend Successful.\n");
		}

	}

	close(sockfd);
	
	return 0;
}
