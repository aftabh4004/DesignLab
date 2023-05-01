#include<stdio.h>
#include<sys/socket.h>
#include<sys/select.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<fcntl.h>
#include<sys/types.h>

#define SERVER_PORT 8000
#define MAX_NAME_SZE 20
#define MAX_BUFFER_SIZE 102400

int child_id;
char client_name[MAX_NAME_SZE] ={0};
char send_buff[MAX_BUFFER_SIZE] ;
char recv_msgg[MAX_BUFFER_SIZE] ;

//Function declaration
int client_create_socket(int *listen_fd, int port);


int main(int argc, char *argv[]) {

    int listen_fd = 0;
    int new_socket = 0;
    int max_fd = 0;
    
    if(argc > 3) {
        perror("ERROR : Parameters error");
        exit(0);
    }
    
    strcpy(client_name,argv[1]);


    if(client_create_socket(&listen_fd, atoi(argv[2])) != 0) {
        perror("ERROR : socket creation failed");
        exit(0);
    }

    child_id=fork();   // creating another process to take input from terminal and other to receive from server

 if(child_id==0)
 {
        while(1){
        
         //in child process handling the terminal input.
        
            memset(send_buff, 0 ,sizeof(send_buff));
            if(read(0,send_buff,sizeof(send_buff))>0) 
            {   //reading data from terminal and sending it to server.
                    
                    if((send(listen_fd, send_buff, MAX_BUFFER_SIZE, 0)) <=0) {
                    perror("ERROR : send failed");
                    return -1;
                              }
            }
        }
            
      
 }       
        

     else{   while(1){  // parent process handling the data received from the server.
                    int bytes_read=0;
                    
                    memset(recv_msgg, 0 ,sizeof(recv_msgg));
                    if((bytes_read=recv(listen_fd, recv_msgg, MAX_BUFFER_SIZE, 0))> 0) {
                        printf("%s\n",recv_msgg);
                         
                         }
               
                         else if(bytes_read == 0) {
                        
                                printf(" DISCONNECTED FROM SERVER");
                                 
                                close(listen_fd);
                                break;
                            
                                 }
                        else {
                                printf("DISCONNECTED");
                               
                                close(listen_fd);  
                                break;   
                }
        }
     }
        
    

    close(listen_fd);


    return 0;
}



//Create a client socket
int client_create_socket(int *listen_fd, int port) {
    struct sockaddr_in server_addr;

    if((*listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("ERROR : socket creation failed");
        return -1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port); 
    server_addr.sin_addr.s_addr = INADDR_ANY;

    
    if(0!=connect(*listen_fd,(struct sockaddr *)&server_addr,sizeof(struct sockaddr))) {
        perror("ERROR : connect failed");
        return -1;
    }
    send(*listen_fd, client_name, MAX_BUFFER_SIZE, 0);
    //client_send_to_server(*listen_fd,client_name);

    return 0;

}

 






