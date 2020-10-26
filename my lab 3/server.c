#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>

//argc = total number of inputs 
//argv the array of inputs
void main(int argc, char **argv) {

    //has to be 2 1-file name 2- port number
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        printf("No port provided\n");
        exit(0);
    }
    printf("%s\n",argv[1]);
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


    //fill the memory at address si_me 
    memset(&si_me, '\0', sizeof (si_me));

    //specify data in the struct for the socket 
    si_me.sin_family = AF_INET;

    //htons converts an IP port number in host to IP port number in network byte order 
    si_me.sin_port = htons(port);
    si_me.sin_addr.s_addr = inet_addr("0.0.0.0");
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    /////bind the socket
    //bind(sockfd, (struct sockaddr*) &si_me, sizeof (si_me));
if ( bind(sockfd, (const struct sockaddr *)&si_me,  
            sizeof(si_me)) < 0 ) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
    //receive a message/data from the socket
    addr_size = sizeof (si_other);

    //buffer is the application buffer in which the data is received
    //1024 is the buffer size
    //0 is Bitwise OR flags, 0 indicates no change to the socket behaviour
    //si_other is the struct for the client socket
    ////addr_size if the size of the client socket structure(si_other)
printf("beforeeee \n");
int n=recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr*) & si_other, &addr_size);
printf("esafawfedgtedswf         %d\n",n);
   /* if (recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr*) & si_other, &addr_size)==-1){
printf("etawertawt\n");
}
else{
printf("buffer %s\n",buffer);
}*/


    printf("Data received: %s\n", buffer);
    if (strcmp(buffer, "ftp") == 0) {
        memset(buffer, 0, sizeof (buffer));
        strcpy(buffer, "yes");
        
        //response message
        sendto(sockfd, buffer, 1024, 0, (struct sockaddr*) & si_other, sizeof (si_other));
        addr_size = sizeof (si_other);
        
        /////////////////////////////////////////SECTION 3
        //receive the packet
        recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr*) & si_other, &addr_size);

        //if the packet is empty
        while (buffer == NULL) {
			
            memset(buffer, 0, sizeof (buffer));
            
            //no packet to acknowledge, respond with NACK
            strcpy(buffer, "NACK");
            sendto(sockfd, buffer, 1024, 0, (struct sockaddr*) & si_other, sizeof (si_other));
            
            //wait for a new packet
            addr_size = sizeof (si_other);
            recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr*) & si_other, &addr_size);
        }

        //copy the packet buffer into this new string, so it stays unchanged
        char* message;
        message = (char *) calloc(sizeof (char), (strlen(buffer) + 1));
        for (int i = 0; i < strlen(buffer); i++) {
            message[i] = buffer[i];
        }
        //printf("the message is %s\n", message);
        memset(buffer, 0, sizeof (buffer));
        
        //acknowledgment
        strcpy(buffer, "ACK");
        sendto(sockfd, buffer, 1024, 0, (struct sockaddr*) & si_other, sizeof (si_other));

        //parse the string to find fragment_numbers, data, filename, etc.
        char delim[] = ":";
        char *ptr = strtok(message, delim);
        int i = 0;
        unsigned int total_frag, frag_no, file_size;
        char* filename;
        while (ptr != NULL) {
  
            //the first value is total_frag
            if (i == 0) {
                total_frag = atoi(ptr);
            } 
            
            //the second value is current fragment_no
            else if (i == 1) {
                frag_no = atoi(ptr);
            } 
            
            //the 3rd element is file_size(max=1000)
            else if (i == 2) {
                file_size = atoi(ptr);
            } 
            
            //the 4th element is the name of the file from which data should be read
            else if (i == 3) {
                filename = (char *) calloc(sizeof (char), (strlen(ptr) + 1));
                for (int i = 0; i < strlen(ptr); i++) {
                    filename[i] = ptr[i];
                }
            }
            ptr = strtok(NULL, delim);
            i++;
        }
        
        //file pointer for reading data from the source file
        FILE * fp_read = fopen(filename, "rb");
        
        //file pointer for writing data to a new binary file
        FILE* fp_write = fopen("new_file.bin", "wb");
		//check if the file exists
        
        //loop through all the fragments
        for (i = 1; i <= total_frag; i++) {
     
            //if the frag_no>1 then the position of the writer file pointer has to be adjusted(so the file is not overwritten and new data is written into the file by each packet)
            if (i > 1) {
                //go to the end of the file, not to overwrite anything, this is done only for i>1, because for the first one we want it to rewrite whatever is already stored in the file
               // fseek(fp_write, 0L, SEEK_END);
            }
            char filedata[file_size];
            
            printf("read %u write %u\n",ftell(fp_read),ftell(fp_write));
            
            //read data from the source file
            //store the data into filedata
            //write filedata into the new binary file
            fread(filedata, sizeof (char), file_size, fp_read);
            fwrite(filedata, sizeof (char), file_size, fp_write);

            //if this is not the last fragment, the server has to still receive more packets
            if (i < total_frag) {
                
                //receive the next packet
                recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr*) & si_other, &addr_size);
                while (buffer == NULL) {
                    memset(buffer, 0, sizeof (buffer));
                    strcpy(buffer, "NACK");
                    sendto(sockfd, buffer, 1024, 0, (struct sockaddr*) & si_other, sizeof (si_other));
                    addr_size = sizeof (si_other);
                    recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr*) & si_other, &addr_size);
                }
                char* message;
                message = (char *) calloc(sizeof (char), (strlen(buffer) + 1));
                for (int i = 0; i < strlen(buffer); i++) {
                    message[i] = buffer[i];
                }
                //printf("%s\n",message);
                memset(buffer, 0, sizeof (buffer));
                strcpy(buffer, "ACK");
                sendto(sockfd, buffer, 1024, 0, (struct sockaddr*) & si_other, sizeof (si_other));
                char delim[] = ":";
                char *ptr = strtok(message, delim);
                int j = 0;
                //int total_frag, frag_no, file_size;
                //char* filename;
                while (ptr != NULL) {
                    if (j == 0) {
                        total_frag = atoi(ptr);
                    } else if (j == 1) {
                        frag_no = atoi(ptr);
                    } else if (j == 2) {
                        file_size = atoi(ptr);
                    } else if (j == 3) {
                        filename = (char *) calloc(sizeof (char), (strlen(ptr) + 1));
                        for (int i = 0; i < strlen(ptr); i++) {
                            filename[i] = ptr[i];
                        }
                    }
                    ptr = strtok(NULL, delim);
                    j++;
                }
            }




        }

        //close the file streams
        fclose(fp_read);
        fclose(fp_write);
        close(sockfd);
    } else {
        memset(buffer, 0, sizeof (buffer));
        strcpy(buffer, "no");
        
        //response message
        sendto(sockfd, buffer, 1024, 0, (struct sockaddr*)& si_other, sizeof(addr_size));
        close(sockfd);
        exit(0);
    }




}
