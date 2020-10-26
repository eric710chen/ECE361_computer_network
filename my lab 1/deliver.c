#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
//#include <arpa/inet.h>

int main(int argc, char const *argv[]) {

	if (argc != 3) {
		printf("Error: wrong argument.\n")
		exit(1);
	}
	
	int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sockfd == -1) {
		printf("Error: socket open at client failed.\n");
		exit(1);
	}

	int portAddr = atoi(argv[1]);
	int portNum = atoi(argv[2]);
	int bufferSize = 256;
	char buffer[bufferSize] = {};
	char fileName[bufferSize] = {};
	int cursor = 0;
	int addrLen = sizeof(serverAddr);
	struct sockaddr_in serverAddr;
	
	memset((char *) &serverAddr, 0, addrLen);
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(portNum);

/*
	if (!inet_aton(argv[1], &serverAddr.sin_addr)) {
		printf("Error: inet_aton error\n");
		exit(1);
	}
*/

	// get input
	printf("Please enter the transfer protocol and file name: ftp <file name> \n");
	gets(buffer, bufferSize, stdin);
	
	while(buffer[cursor] == " ") {
		cursor++;
	}
	if ((buffer[cursor] == "f") && (buffer[cursor + 1] == "t") && (buffer[cursor + 2] == "p")) {
		cursor = cursor + 3;
		while (buffer[cursor] == " ") {
			cursor++;
		}
		//char *token = strtok(buffer + cursor, "\r\t\n");
		//strncpy(fileName, token, bufferSize);
	} else {
		printf("Error: ftp not found.\n");
		exit(1);
	}

	int accessable = access(fileName, F_OK);
	if (accessable == -1) {
		printf("Error: file does not exist.\n")
		exit(1);
	}

	// timer begins
	clock_t timer = clock();

	// sent message
	addrLen = sizeif(serverAddr);
	int sent = sendto(sockfd, "ftp", strlen("ftp"), 0, (struct sockaddr *) &serverAddr, addrLen);
	if (sent == -1) {
		printf("Error: something wrong with sendto.\n");
		exit(1);
	}

	// clear the buffer
	memset(buffer, 0, bufferSize);
	
	// receive message
	socklen_t serverAddrSize = sizeof(serverAddr);
	int received = recvfrom(sockfd, buffer, bufferSize, 0, (struct sockaddr *) &serverAddr, &serverAddrSize);
	if (received == -1) {
		printf("Error: something worng with receivedFrom.\n");
		exit(1);
	}

	int success = strcmp("yes", buffer);
	if (success) {
		printf("A file transfer can start.\n");
	}

	// timer ends
	timer = clock() - timer;
	float s = (float)time * 1000000 / CLOCKS_PER_SEC;
	printf("The round-trip time is %.4f second.\n", s);

	close(sockfd);

	return 0;
}
