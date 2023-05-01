#include <stdio.h>
#include <sys/types.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>


#define BUF_LEN 256

// Global sockfd 
int sockfd;

void error(char *msg)
{
  perror(msg);
  exit(0);
}


int send_quit = 0;

void sigDHandler(){
    exit(0);
}
void sigCHandler(){

    char buffer[BUF_LEN];
    sprintf(buffer, "/quit");
    send(sockfd,buffer, sizeof(buffer), 0); 
}
int main(int argc,char *argv[])
{
    signal(SIGINT, sigCHandler);
    signal(SIGTSTP, sigCHandler);
    signal(SIGQUIT, sigDHandler);



    int portno= 5000;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct hostent *server;
    server = gethostbyname("127.0.0.1");

    if (server == NULL){
        fprintf(stderr,"ERROR, no such host");
        exit(0);
    }

    struct sockaddr_in serv_addr;
    bzero((char *) &serv_addr, sizeof(serv_addr)); // initializes buffer
    serv_addr.sin_family = AF_INET; // for IPv4 family
    bcopy((char *)server->h_addr, (char *) &serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno); //defining port number

    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0){
         error("ERROR connecting");
    }

    int pid;

    if((pid = fork()) != 0){
        //child process, will be use for reading
        while(1){
            char buffer[BUF_LEN];
            bzero(buffer, BUF_LEN);
			recv(sockfd, buffer, sizeof(buffer), 0);
            if (strncmp(buffer, "STOP", 4) == 0){
                kill(pid, SIGQUIT);
                printf("\nBye!\n");
                break;
            }
            
            printf("%s", buffer);
            
        }
    }
    else{
        //Process to wrrite
        while(1){
            char buffer[BUF_LEN];
            bzero(buffer,BUF_LEN);
            fgets(buffer,BUF_LEN,stdin);
            if (send_quit) {
                
            }
	        send(sockfd,buffer, sizeof(buffer), 0);   

        }
    }
    close(sockfd);
    return 0;
}
