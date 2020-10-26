#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>

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
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
printf("%s hiii\n",argv[1]);
printf("%s hiisdgtsedtystyi\n",server_address);
printf("%u this shoulsedfwerf \n",server_addr.sin_addr.s_addr);
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
    char * filename;
    //const char *filename_backup = "deliver.c";
    char* filename_backup;
    char *p = strtok(file, " ");
    if (strcmp(p, "ftp") == 0) {

        while (p != NULL) {
            filename = p;
            filename = (char *) calloc(sizeof (char), (strlen(p) + 1));
            for (int i = 0; i < strlen(p); i++) {
                filename[i] = p[i];
            }
            filename_backup = (char *) calloc(sizeof (char), (strlen(p) + 1));
            for (int i = 0; i < strlen(p); i++) {
                filename_backup[i] = p[i];
            }
            p = strtok(NULL, " ");
        }

    }

    //if file exists
    if (access(filename, F_OK) != -1) {
        strcpy(buffer, "ftp");
        printf("file found %s", filename);
    } else {
        printf("file not found %s\n", filename);
        close(sockfd);
        exit(0);
    }
    
    /////////////////////////////////////////SECTION 2///////////////////////////////////////////
    //measuring RTT
    clock_t start0, end0, start1, end1;
	int RTT = 1 * CLOCKS_PER_SEC;
    
    //set clock before the data is sent to the server
    start0 = clock();
    
    //send the data
    sendto(sockfd, buffer, 1024, 0, (struct sockaddr*) &server_addr, sizeof (server_addr));
	
    
    //set the clock after data is sent
    end0 = clock();
    
    //RTT
    double round_trip_time = ((double) (end0 - start0)) / CLOCKS_PER_SEC;
    printf("Measured Round Trip Time = %f", round_trip_time);
    printf("Data sent: %s\n", buffer);
    addr_size = sizeof (server_addr);
    recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr*) &server_addr, &(addr_size));

    if (strcmp(buffer, "yes") == 0)
        printf("A file transfer can start.\n");

    else {
        close(sockfd);
        exit(0);
    }

    struct packet {
        unsigned int total_frag;
        unsigned int frag_no;
        unsigned int size;
        char* filename;
        char filedata[1000];
    };


    if (filename_backup == '\0') {
        printf("yes\n");
    } else {
        printf("no%s", filename_backup);
    }
    fflush(stdout);
    
    //file pointer to read from the file
    FILE * fp = fopen(filename_backup, "r");

    //seek the end of the file
    fseek(fp, 0, SEEK_END);

    //the position at which the file pointer is at is equal to the file size
    unsigned int file_size = ftell(fp);

    //trace back to the beginning of the file
    fseek(fp, 0, SEEK_SET);
    
    //fragment numbers start from 1
    unsigned int frag_numbers = 1;
    
    //total fragments=file_size/1000 + 1(in case the last file is less than 1000bytes in size)
    unsigned int total_frag = file_size / 1000;
    //printf("total size is %u\n", file_size);
    //printf("total_frag is %u \n", total_frag);
    
    //add 1 for the last file (its size is less than 1000bytes))
    if ((file_size % 1000) > 0)
        total_frag++;
    
    //after each packet is sent, file_size is decremented by 1000, so this continues as long as there is more than 1 packet pending
    while (file_size > 1000) {
    
        
        //the char array that is going to be sent to the server
        //since the file_size is still >1000, the size of each packet is at maximum(1000)
        char filedata[1000];

        //read 1000 bytes from the file and store it into the char array
        fread(filedata, sizeof (char), 1000, fp);

        //create a packet
        struct packet user_req;

        //set the number of total fragments
        user_req.total_frag = total_frag;

        //the fragment number of the current packet
        user_req.frag_no = frag_numbers;
        frag_numbers++;
        //const char*filename_backup = "deliver.c";
        printf("%s \n", filename_backup);
        
        //set the filename of the packet
        user_req.filename = (char*) calloc(sizeof (char), (strlen(filename_backup) + 1));
        for (int i = 0; i < strlen(filename_backup); i++) {
            user_req.filename[i] = filename_backup[i];
        }

        //size of the current packet
        user_req.size = 1000;
        for (int i = 0; i < 1000; i++) {
            
           //the data string in of the struct(the packet))
            user_req.filedata[i] = filedata[i];
        }

        //allocate memory for the buffer
        memset(buffer, 0, sizeof (buffer));
        char temp[1024];
        
        //convert number to string
        sprintf(temp, "%u", user_req.total_frag);
        
        //add the string to the buffer
        strcpy(buffer, temp);
        strcat(buffer, ":");
        sprintf(temp, "%u", user_req.frag_no);
        strcat(buffer, temp);
        strcat(buffer, ":");
        sprintf(temp, "%u", user_req.size);
        strcat(buffer, temp);
        strcat(buffer, ":");
        strcat(buffer, user_req.filename);
        strcat(buffer, ":");
        strcat(buffer, user_req.filedata);
        
        //file_size-1000 to indicate the size of the remaining data that is to be sent to the server
        file_size -= 1000;
        
		start1 = clock();

        //send the packet
        sendto(sockfd, buffer, 1024, 0, (struct sockaddr*) &server_addr, sizeof (server_addr));	
		
        //wait for ACK
        addr_size = sizeof (server_addr);

		bool received = false;
		while(!received) {
			end1 = clock();
			while((end1 - start1) < RTT) {
        		recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr*) &server_addr, &(addr_size));
        		if (strcmp(buffer, "ACK") == 0) {
            		printf("ACK => File transfer can continue.\n");
					received = true;
				}
        		//if the packet is not acknowledged(as long as receiving 'NACK'), keep resending the packets
			}
			if ((received == false)) {
				start1 = clock();
        		//resend the packet
       			printf("Lost of packet at the server due to timeout. Resend the packet\n" );
           		sendto(sockfd, buffer, 1024, 0, (struct sockaddr*) &server_addr, sizeof (server_addr));
           		//wait for ACK
           		//addr_size = sizeof (server_addr);
           		//recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr*) &server_addr, &(addr_size));
       		}
		}
		end1 = clock();
		RTT = ((int) (end1 - start1)) * 2;
		if (RTT > (3 * CLOCKS_PER_SEC)) {
			RTT = 3 * CLOCKS_PER_SEC;		
		}

        //change the position of the filestream and go to the next 1000bytes of the file
        //fseek(fp, 1000, SEEK_CUR);
        printf("%u",ftell(fp));

    }
    
    //at this point, only the last packet is remaining 
    //the size of the filedata is not at max(1000) anymore, since the last packet is less than 1000bytes in size
    char filedata[file_size];

    //read the remaining bytes from the file and store it into the char array
    fread(filedata, sizeof (char), file_size, fp);
    struct packet user_req;
    user_req.total_frag = total_frag;
    user_req.frag_no = frag_numbers;
    user_req.filename = (char*) malloc(sizeof (filename_backup) / sizeof (char));
    for (int i = 0; i < (sizeof (filename_backup) / sizeof (char)); i++) {
        user_req.filename[i] = filename_backup[i];
    }
    user_req.size = file_size;
    for (int i = 0; i < file_size; i++) {
        user_req.filedata[i] = filedata[i];
    }
    memset(buffer, 0, sizeof (buffer));
    char temp[1024];
    sprintf(temp, "%u", user_req.total_frag);
    strcpy(buffer, temp);
    strcat(buffer, ":");
    sprintf(temp, "%u", user_req.frag_no);
    strcat(buffer, temp);
    strcat(buffer, ":");
    sprintf(temp, "%u", user_req.size);
    strcat(buffer, temp);
    strcat(buffer, ":");
    strcat(buffer, user_req.filename);
    strcat(buffer, ":");
    strcat(buffer, user_req.filedata);
    //printf("%s\n",buffer);
    sendto(sockfd, buffer, 1024, 0, (struct sockaddr*) &server_addr, sizeof (server_addr));
    addr_size = sizeof (server_addr);
    recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr*) &server_addr, &(addr_size));
    if (strcmp(buffer, "ACK") == 0)
        printf("ACK => All files sent.\n");
    else {
        while (strcmp(buffer, "NACK") == 0) {
            printf("NACK => Files are not received at the server. Resend the packet\n");
            sendto(sockfd, buffer, 1024, 0, (struct sockaddr*) &server_addr, sizeof (server_addr));
            addr_size = sizeof (server_addr);
            recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr*) &server_addr, &(addr_size));
        }
    }

    //close the filestream
    fclose(fp);
    
    //note that we do not need to bind the socket to address for client in UDP, it is because of UDP
    free(file);
    close(sockfd);
}
