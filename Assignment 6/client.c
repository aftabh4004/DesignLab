#include <stdio.h>
#include <sys/types.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void error(char *msg)
{
  perror(msg);
  exit(0);
}

int main(int argc,char *argv[])
{
    int sockfd, portno= 5000;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct hostent *server;
    server = gethostbyname("127.0.0.1");
    if (server == NULL)
   {
    fprintf(stderr,"ERROR, no such host");
    exit(0);
   }
    struct sockaddr_in serv_addr;
    bzero((char *) &serv_addr, sizeof(serv_addr)); // initializes buffer
    serv_addr.sin_family = AF_INET; // for IPv4 family
    bcopy((char *)server->h_addr, (char *) &serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno); //defining port number
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
    {
         error("ERROR connecting");
    }
    char buffer[256];
    printf("Enter the commands and type STOP to quit\n");
    while(1)
    {
    bzero(buffer,256);
    int n; // client buffer to forward request to the server
        bzero(buffer,256);

        bzero(buffer,256);
        fgets(buffer,256,stdin);
        n=write(sockfd,buffer,sizeof(buffer));


    
    n=read(sockfd,buffer,sizeof(buffer));
    
    if(strncmp(buffer,"STREAM",6)==0){
        n=read(sockfd,buffer,sizeof(buffer));

        while (strncmp(buffer,"DONE",4)!=0){
            printf("%s", buffer);
            bzero(buffer, 256);
            n=read(sockfd,buffer,sizeof(buffer));   
        }
        bzero(buffer, 256);
        printf("\n");
        continue;
        
    }

    if(strncmp(buffer,"CONNECTION TERMINATED",21)==0){
        printf("***SUCCESSFUL TERMINATION***\n");
        break;
    }
    printf("%s\n",buffer);
    bzero(buffer,256);



        }
    close(sockfd);
    return 0;
}
