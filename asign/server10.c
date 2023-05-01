/*We have just provided a skeleton code. Feel free to add/remove functiolaties as you like. The function definitions are not complete. Please provide appropriate parameters and return types to a function in case you decide to use it in your submission.*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>	
#include <time.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <errno.h>
#include <sys/select.h>
#include <arpa/inet.h>




#define listen_backlog 5
#define maxName_size 20
#define max_clients 5
#define max_buffer  5000



static int listen_fd=0;
static int i=89999;
static int h=300000;
static int x=500000;
int y=0; // to count clients sockets in makereq group.

//data structure to store client details

struct gp_data{
    int clt_sockt[5];
    char grp_id[10];
    int grp_admin;
    int broadcast;

};
struct group_data{
     int nogps;
     struct gp_data groups[maxName_size];
};

struct group_data finalgrp;  //group formed after receiving all the requests
struct group_data makegp;
struct req_group{
    int clnt_sct[5];
    char grp_unid[10];
    int req_admin;
    int no_of_reqs;
    int clnto_indx;
    int total_clients;

};

struct requesting_group{
       int nreq_grps;
       struct req_group r_gp[max_clients];

};

struct requesting_group ask;
//struct gp_data r_grpFormed[maxName_size];



//struct gp_data gp_info[maxName_size];
//struct gp_data req_gp[maxName_size];


//static int ttl_grps;



struct client_data{
      char client_name[maxName_size];
      char chat_to[maxName_size];
      int chat_toFD;
      int filedes;
      int port;
      char client_id[5];
     char ip[INET_ADDRSTRLEN];
      

};

struct server_data{
       int all_clients;
       struct client_data client_list[max_clients];

};

struct server_data serverside;
 


    
//DECLARING FUNCTIONS------------------------------------------------------

int form_socket(int *listen_fd,int port_no);
int handling_new_client(int listen_fd, int *new_socket_fd);
int receive_data_from_client(int socket_client, char *recv_msg);
int sending_data_to_client(int socket_client, char *send_msg);

int populate_fdsets(int listenfd,fd_set *readfds,fd_set *writefds,fd_set *exceptfds);
int selection_of_clients(int max_fd,int listenfd,fd_set *readfds,fd_set *writefds);

int server_add_new_client(struct sockaddr_in client_info,int new_socket_fd);
int process_data_from_client(int socket,char *buffer);
int search_clientIndexby_socket(int socket);
int search_clientIndexby_uniID(char*uid);
void cleanup(void);
// FUNCTION DECLARATION----------------------------------------------------------


int main(int argc,char *argv[])
{
   int new_socket=0;
   int server_port_no;
   int joint_req_sent[5];
   fd_set readfds;
   fd_set writefds;
   fd_set exceptfds;
   int maxfd=0;
   if(argc<2)
   {
    fprintf(stderr,"ERROR, no port provided\n");
		exit(1);
   }
    server_port_no=atoi(argv[1]);
   memset(&serverside,0,sizeof(struct server_data));
   printf("------------SERVER IS WORKING-----------\n");

   if(form_socket(&listen_fd,server_port_no)!=0)
   {
      perror("SOCKET CREATION FAILED");
      exit(0);
   }

   maxfd=listen_fd;
   while(1)
   {
    maxfd=populate_fdsets(listen_fd, &readfds, &writefds, &exceptfds);
    selection_of_clients(maxfd, listen_fd,&readfds,&writefds);


   }

   return 0;

}

//select system call
int selection_of_clients(int max_fd, int listen_fd,fd_set *readfds,fd_set *writefds)
{
    int new_socket_fd=0;
  //---------------------------------
    // unsigned int seed=12345;
    // srand(seed);
    // int uniq_id=rand()%16977;     //generate the random number
    // char welcme_msg[300];  // store the welcome message
    // char id[5];
    // sprintf(id,"%d",uniq_id);
    // strcpy(welcme_msg,id);
    // strcat(welcme_msg,":");
    // strcat(welcme_msg,"WELCOME TO MY WORLD!");
    //---------------------------------------------------
    char recv_msgg[max_buffer];
    char send_buff[max_buffer];
    memset(recv_msgg,0,sizeof(recv_msgg));
    memset(send_buff,0,sizeof(send_buff));

    int action=select(max_fd+1,readfds,writefds,NULL,NULL);
    if(action==-1 || action ==0)
    {
        perror("SELECT ERROR");
        exit(0);
    }
    //check the server listenfd
    if(FD_ISSET(listen_fd,readfds))
    {
        int h=handling_new_client(listen_fd,&new_socket_fd);
        if(h!=-1)
        printf("NEW SOCKET CREATED=%d\n",new_socket_fd);
        //sending_data_to_client(listen_fd,welcme_msg);


    }

    //check the data from stdin and send message to all connected clients
    if(FD_ISSET(STDIN_FILENO,readfds))
    {
        if(read(0,send_buff,sizeof(send_buff))>0)
        {
            for(int i=0;i<serverside.all_clients;i++)
            {
                sending_data_to_client(serverside.client_list[i].filedes,send_buff);
            }
        }
    }

        for(int i=0;i<serverside.all_clients;i++)   
            {
                 if(FD_ISSET(serverside.client_list[i].filedes,readfds))
                {
                    receive_data_from_client(serverside.client_list[i].filedes,recv_msgg);  
                }
            }


    return 0;

}

//populating fd sets---
int populate_fdsets(int listen_fd,fd_set *readfds, fd_set *writefds,fd_set *exceptfds)
{
     int max_fd=listen_fd;
     FD_ZERO(readfds);
     FD_SET(listen_fd,readfds);
     FD_SET(STDIN_FILENO,readfds);
     fcntl(STDIN_FILENO,F_SETFL,O_NONBLOCK);

     for(int i=0;i<serverside.all_clients;i++)
     {
        FD_SET(serverside.client_list[i].filedes,readfds);
        max_fd++;
     }
     return max_fd;
     
}
	

//creating server socket
int form_socket(int *listen_fd, int port_no)
{
    struct sockaddr_in server_add;
    if((*listen_fd=socket(AF_INET,SOCK_STREAM,0))==-1)
    {
        perror("SOCKET CREATION FAILED");
        return -1;
    }

    server_add.sin_family=AF_INET;
    server_add.sin_port=htons(port_no);
    server_add.sin_addr.s_addr=INADDR_ANY;

    if (bind(*listen_fd, (struct sockaddr*) &server_add, sizeof(server_add))!= 0){
		perror("ERROR on binding");
        return -1;
 	}

    if(listen(*listen_fd,listen_backlog)!=0)
    {
        perror("ERROR on listening");
        return -1;
    }
    return 0;


}
//receiving socket data from clients
int receive_data_from_client(int client_socket,char *recvmsg)
{
    int read_bytes=0;
    int close_client_index=0;
    memset(recvmsg,0,strlen(recvmsg));
    if((read_bytes=recv(client_socket,recvmsg,max_buffer,0))>0)
    {   
        process_data_from_client(client_socket,recvmsg);
    }
    else if(read_bytes==0)
    {
        printf("CLIENT DISCONNECTED\n");
         close_client_index=search_clientIndexby_socket(client_socket);
           int i;
            for(i=close_client_index;i<max_clients;i++)
             {
                serverside.client_list[i]=serverside.client_list[i+1];

             }
             serverside.all_clients--;
           printf("SOCKET SUCCESSFULLY CLOSED");
           close(client_socket);
       
    }
    else
    {
        printf("Receive has failed\n");
    }
    
    return 0;

}


int sending_data_to_client(int client_socket, char *send_msg) {
    int write_bytes = 0;
    int len =strlen(send_msg);
    
    if((write_bytes = send(client_socket, send_msg, max_buffer, 0)) > 0) {
            printf("\n[CLIENT : %d ] || MESSAGE SENT= [%s]\n",client_socket, send_msg);
    }
    else {
            perror("Error : send failed");
            return -1;
    }
   
    return write_bytes;
}
//find index of the client data structure from client unique_id-------------------------------
int search_clientIndexby_uniID(char *uid) {
    int index = -1;
    
    for(int i = 0; i<serverside.all_clients; i++) {
           
           if(strncmp(serverside.client_list[i].client_id,uid,5) == 0) {
               index =i;
           }
    }

    
    return index;
}
//----------------------------------------------------------------------------

//TO PROCESS RECEIVED DATA FROM CLIENTS
int process_data_from_client(int socket,char*buffer) {
    char chat_c[max_buffer];
    //int join_received[5];
    char buffer_send[max_buffer] = {0};
    int requested_clients[5];   // array of requested clients
    int index_sender = 0;            
    int index_receiver = 0;
    char invalid_uid[500];
    static int ttl_grps=0;
    static int req_gp_indx=0;
    
    
    int len = 0;
    index_sender = search_clientIndexby_socket(socket);
     char *newbuffer=NULL;
     if(buffer!=NULL)
         newbuffer=strtok(buffer," ");   //keyword
         if(newbuffer!=NULL){
         
    if(strncmp(newbuffer,"/activegroups",13) ==0) {
         memset(newbuffer,0,sizeof(newbuffer));
         int k=0; // iterator for client socket.
         int h=0;
         int w=0;
         int x=0;
         int mem_indx=0;
         char mem_id[5];
         char mem_idn[5];
         
         
         for(int i=0;i<finalgrp.nogps;i++) //first time condition is not true.
         {  // ITERATING OVER WITH REQUEST GROUP.
                  k=0; //for every new group.
                for(int k = 0; k < 5; k++)
                {
                    if(finalgrp.groups[i].clt_sockt[k]==0) continue; // checks if request sender is present in a group
               
                    if(finalgrp.groups[i].clt_sockt[k]==socket)
                    {
                        strcat(newbuffer,"GROUP ID:");
                        strcat(newbuffer,finalgrp.groups[i].grp_id);
                        strcat(newbuffer,"  GROUP ADMIN:");
                            mem_indx=search_clientIndexby_socket(finalgrp.groups[i].grp_admin);
                            strcpy(mem_id,serverside.client_list[mem_indx].client_id);
                        strcat(newbuffer,mem_id);
                         memset(mem_id,0,sizeof(mem_id));
                        strcat(newbuffer,"  CLIENT IDS:");
                        h=0;
                        for(h = 0; h < 5; h++) 
                        {
                            if(finalgrp.groups[i].clt_sockt[h]==0) continue;
                            strcat(newbuffer,"  |");
                            mem_indx=search_clientIndexby_socket(finalgrp.groups[i].clt_sockt[h]);
                             strcpy(mem_id,serverside.client_list[mem_indx].client_id);
                              
                            strcat(newbuffer,mem_id);
                            memset(mem_id,0,sizeof(mem_id));
                        } 
                        break;  

                    }
                            
                }
            
         }
         for(int l=0;l<makegp.nogps;l++) 
         {  // ITERATING OVER WITHOUT REQUEST GROUP.
               w=0;
                for(w = 0; w < 5; w++)  // checks if request sender is present in a group
               {   
                    if(makegp.groups[l].clt_sockt[w]==0) continue;
                    if(makegp.groups[l].clt_sockt[w]==socket)
                    {
                        strcat(newbuffer,"GROUP ID:");
                        strcat(newbuffer,makegp.groups[l].grp_id);
                        strcat(newbuffer,"  GROUP ADMIN:");
                        mem_indx=search_clientIndexby_socket(makegp.groups[l].grp_admin);
                            strcpy(mem_idn,serverside.client_list[mem_indx].client_id);
                        strcat(newbuffer,mem_idn);
                        memset(mem_idn,0,sizeof(mem_idn));
                        strcat(newbuffer,"  CLIENT IDS:");
                        x=0;
                        for(x = 0; x < 5;x++)
                        {
                            if(makegp.groups[l].clt_sockt[x]==0) continue;
                        
                            strcat(newbuffer,"  |");
                            mem_indx=search_clientIndexby_socket(makegp.groups[l].clt_sockt[x]);
                            
                             strcpy(mem_idn,serverside.client_list[mem_indx].client_id);
                             strcat(newbuffer,mem_idn);
                                memset(mem_idn,0,sizeof(mem_idn));
                            
                        } 
                        break;  

                    }
                        
                        
                }
                 strcat(newbuffer,"|\n");
         
            
         }
        sending_data_to_client(socket,newbuffer);
        memset(newbuffer,0,sizeof(newbuffer));
        return 0;
        // goto out;
    }

    
         
    if(strncmp(newbuffer, "/active",7) ==0) {

         memset(newbuffer,0,sizeof(newbuffer));
         for(int i=0;i<serverside.all_clients;i++) {
             strcat(newbuffer,"CLIENT: ");
             strcat(newbuffer,serverside.client_list[i].client_name);
             strcat(newbuffer," UNIQUE ID: ");
             strcat(newbuffer,serverside.client_list[i].client_id);
             strcat(newbuffer," || ");
         }
        sending_data_to_client(socket,newbuffer);
        memset(newbuffer,0,sizeof(newbuffer));
        return 0;
        // goto out;
    }
    if(strncmp(newbuffer,"/addtogroup",10) == 0)
     {
           char *client1;
          char *gp_id=strtok(NULL," ");  //group id
          printf("|%s| group id\n", gp_id);
          if(gp_id == NULL){
            sending_data_to_client(socket,"Need Group ID\n");
            return 0;
          }

           for(int i=0;i<makegp.nogps;i++)
           {
            int gid = atoi(gp_id);
            int gid2 = atoi(makegp.groups[i].grp_id);
            if(gid == gid2)
            {
                if(makegp.groups[i].grp_admin!=socket){
                    sending_data_to_client(socket,"YOU ARE NOT AN ADMIN");
                    return 0;
                }
                
                client1=strtok(NULL," ");

                while(client1){
                    printf("insider |%s|", client1);
                    int client_id=0;
                    int socket_c ;
                    if(client1!=NULL)
                    {
                        client_id=search_clientIndexby_uniID(client1);

                        socket_c=serverside.client_list[client_id].filedes;
                        printf("socker|%d|\n", socket_c);

                        int flag = 0;
                        for(int i=0;i<5;i++){
                            if(makegp.groups->clt_sockt[i] == 0){
                                flag = 1;
                                makegp.groups->clt_sockt[i] = socket_c;
                                
                                
                                char buffer[256];
                                bzero(buffer, 256);
                                sprintf(buffer, "You have been added to  group %s\n", gp_id);
                                printf("You have been added to  group%s\n", gp_id);
                                printf("socker2|%d|\n", socket_c);
                                sending_data_to_client(socket_c,buffer);

                                client1 = strtok(NULL, " ");
                                break;
                                
                            }
                        }
                        if (flag == 0){
                            char buffer[256];
                            bzero(buffer, 256);
                            sprintf(buffer,"%s\n", "Group is already full");
                            sending_data_to_client(socket,buffer);
                        }

                    }
                    client1 = strtok(NULL, " ");
                }
                break;
             
            }
            
           }
             




          
     }
    if(strncmp(newbuffer,"/removefromgroup",10) == 0)
     {
           char *client1;
          char *gp_id=strtok(NULL," ");  //group id
          printf("|%s| group id\n", gp_id);
          if(gp_id == NULL){
            sending_data_to_client(socket,"Need Group ID\n");
            return 0;
          }

           for(int i=0;i<makegp.nogps;i++)
           {
            int gid = atoi(gp_id);
            int gid2 = atoi(makegp.groups[i].grp_id);
            if(gid == gid2)
            {
                if(makegp.groups[i].grp_admin!=socket){
                    sending_data_to_client(socket,"YOU ARE NOT AN ADMIN");
                    return 0;
                }
                
                client1=strtok(NULL," ");

                while(client1){
                    printf("insider |%s|", client1);
                    int client_id=0;
                    int socket_c ;
                    if(client1!=NULL)
                    {
                        client_id=search_clientIndexby_uniID(client1);

                        socket_c=serverside.client_list[client_id].filedes;
                        printf("socker|%d|\n", socket_c);

                        int flag = 0;
                        for(int i=0;i<5;i++){
                            if(makegp.groups->clt_sockt[i] == socket_c){
                                flag = 1;
                                makegp.groups->clt_sockt[i] = 0;
                                int m = i + 1;
                                while(m < 5){
                                    makegp.groups->clt_sockt[m - 1] = makegp.groups->clt_sockt[m];
                                    m++;
                                }
                                makegp.groups->clt_sockt[4] = 0;
                                char buffer[256];
                                bzero(buffer, 256);
                                sprintf(buffer, "You have been removed from group %s\n", gp_id);
                                printf("You have been removed from group %s\n", gp_id);
                                printf("socker2|%d|\n", socket_c);
                                sending_data_to_client(socket_c,buffer);

                                client1 = strtok(NULL, " ");
                                break;
                                
                            }
                        }
                        if (flag == 0){
                            char buffer[256];
                            bzero(buffer, 256);
                            sprintf(buffer, "%s was not in the group\n", client1);
                            sending_data_to_client(socket,buffer);
                        }

                    }
                    client1 = strtok(NULL, " ");
                }
                break;
             
            }
            
           }
             




          
     }
          
            if(strncmp(newbuffer,"/sendgroup",10) == 0)
                {
                        char *uid=strtok(NULL," ");       // id no.
                         char *msgrcvd=strtok(NULL,"");   //msg
                         int client_indx=0;
                         char notingp[90];
                        // strcpy(notingp,"CLIENT NOT IN GROUP");

                    for(int j=0;j<makegp.nogps;j++)
                    {
                        if(strncmp(makegp.groups[j].grp_id,uid,6)==0){
                        for(int i=0;i<5;i++)
                        {
                            
                            if(makegp.groups[j].clt_sockt[i]==socket || makegp.groups[j].grp_admin==socket)
                            {
                                if(makegp.groups[j].broadcast==1)
                                {
                                    if(makegp.groups[j].grp_admin==socket)
                                    {
                                        for(int v=0;v<5;v++)
                                        {   
                                            if(makegp.groups[j].clt_sockt[v]!=socket)
                                            sending_data_to_client(makegp.groups[j].clt_sockt[v],msgrcvd);
                                        }
                                    }
                                    else
                                    {
                                        sending_data_to_client(socket,"You are not an admin\n");
                                    }
                                }
                                else
                                {
                                    for(int v=0;v<5;v++)
                                        {   
                                            if(makegp.groups[j].clt_sockt[v]!=socket)
                                            sending_data_to_client(makegp.groups[j].clt_sockt[v],msgrcvd);
                                        }

                                }
                                break;
                            }
                            else
                            {
                               sending_data_to_client(socket,"YOU ARE NOT MEMBER OF THE GROUP");
 
                            }
                            
                        }
                        }

                    }
                   return 0; 
                }
    if(strncmp(newbuffer,"/send",5) == 0) {
        
        // char *name=strtok(newbuffer," ");
        //  name=strtok(NULL," ");
        
        char *uid=strtok(NULL," ");       // id no.
        char *msgrcvd=strtok(NULL,"");     //message
         strcpy(serverside.client_list[index_sender].chat_to, uid);
        // sscanf(buffer,"%*[^:]:%s",chat_c);
        // printf("broadcast---->%s ",chat_c);
        // strcpy(serverside.client_list[index_sender].chat_to, chat_c);
       
        index_receiver = search_clientIndexby_uniID(serverside.client_list[index_sender].chat_to);
        if(index_receiver==-1)   // if uid entered by the client is invalid.
        {
            strcpy(invalid_uid,"NO CLIENT WITH SUCH UID");   
            sending_data_to_client(serverside.client_list[index_sender].filedes,invalid_uid);
        }
        else{
        serverside.client_list[index_sender].chat_toFD = serverside.client_list[index_receiver].filedes;
        if(strlen(serverside.client_list[index_sender].chat_to) != 0){
         snprintf(buffer_send,sizeof(buffer_send),"[%s--%s] : %s",serverside.client_list[index_sender].client_name,serverside.client_list[index_sender].client_id,msgrcvd);
        //printf("Buffer  =%s\n",buffer_send);
        sending_data_to_client(serverside.client_list[index_sender].chat_toFD,buffer_send);
         }}
         return 0;
        //sending_data_to_client(serverside.client_list[index_sender].filedes,connected);
        //goto out;
    }

      if(strncmp(newbuffer,"/broadcast",10)==0)
      {
            
             char *first=strtok(NULL,"");   //message
             for(int i=0;i<serverside.all_clients;i++)
             {
                 snprintf(buffer_send,sizeof(buffer_send),"[%s--%s] : %s",serverside.client_list[index_sender].client_name,serverside.client_list[index_sender].client_id,first);
                 if(i!=index_sender)
                 sending_data_to_client(serverside.client_list[i].filedes,buffer_send);
             }  
             return 0;
      }
       if(strncmp(newbuffer,"/quit",5) == 0)
       {
          memset(buffer,0,sizeof(buffer));
          int close_client_index=search_clientIndexby_socket(socket);
           int i;
           char to_quit[400];
           strcpy(to_quit,"CONNECTION CLOSED");
           
           sending_data_to_client(socket,to_quit);
           close(socket);
            for(i=close_client_index;i<max_clients;i++)
             {
                serverside.client_list[i]=serverside.client_list[i+1];

             }
             serverside.all_clients--;
             
           printf("SOCKET SUCCESSFULLY CLOSED");
           
           
            
       }
        if(strncmp(newbuffer,"/makegroupbroadcast",19) == 0)
        {
                newbuffer=strtok(NULL," "); //group_id
                if(newbuffer!=NULL)
                {
                    for(int i=0;i<makegp.nogps;i++)
                    {
                        if(strncmp(makegp.groups[i].grp_id,newbuffer,6)==0)
                         {
                            if(makegp.groups[i].grp_admin==socket)
                              makegp.groups[i].broadcast=1;

                              else
                               sending_data_to_client(socket,"YOU ARE NOT AN ADMIN");

                         }
                    }
                }
                else
                {
                   sending_data_to_client(socket,"INVALID REQUEST"); 
                }

        }

      
            if(strncmp(newbuffer,"/makegroupreq",13) == 0)
            { 
                
                unsigned int seed2=10000; 
                int grp_uniq_id2;
                char req_to_clnt[90];
                
                char info2[250];
                char grp_join2[200];  
                srand(seed2);
                
                grp_uniq_id2=rand()%x;     //generate the random number
                x=x+2;
                char grp_key2[5];
                //char newbuffer[max_buffer];
                char gr_k2[5];
                
                int clint_indx2=0;
                sprintf(grp_key2,"%d",grp_uniq_id2);
                 memset(req_to_clnt,0,sizeof(req_to_clnt));
                 sprintf(req_to_clnt,"REQUESTED TO JOIN %d",grp_uniq_id2);
                //strcpy(req_to_clnt,"REQUESTED");
                
              // ADD GROUP ADMIN TO THE GROUP.
                
                 strcpy(ask.r_gp[ask.nreq_grps].grp_unid,grp_key2);
                 ask.r_gp[ask.nreq_grps].req_admin=socket;
                 ask.r_gp[ask.nreq_grps].no_of_reqs++;  // keeps count of number of requests.
                 ask.nreq_grps++; // keeps count of current number of groups.
                            
               memset(info2,0,sizeof(info2));
              sprintf(info2,"Members are being requested to join : GROUP ID: %d",grp_uniq_id2);
              sending_data_to_client(socket,info2);
               int clnt_socket;
                 while(newbuffer!=NULL)
                 {
                     newbuffer=strtok(NULL," ");    // client id
                     if(newbuffer!=NULL)
                        { 
                          clint_indx2=search_clientIndexby_uniID(newbuffer);
                          clnt_socket=serverside.client_list[clint_indx2].filedes;
                         // printf("sending request message to client-->%d",clnt_socket);
                         //store client socket to structure.
                            ask.r_gp[ask.nreq_grps].clnt_sct[ask.r_gp[ask.nreq_grps].clnto_indx++]=clnt_socket; 
                            
                            sending_data_to_client(clnt_socket,req_to_clnt);  //sending request message to client.
                            ask.r_gp[ask.nreq_grps].total_clients++;

                        }      

                 }
                 
                 

                 return 0;

            }
         
    
    if(strncmp(newbuffer,"/joingroup",10) == 0)
                {
                    newbuffer=strtok(NULL," ");  //group_id
                    char infoadded[90];
                    int gp_m=0;
                    sprintf(infoadded,"You are added to the group-->%s",ask.r_gp[ask.nreq_grps].grp_unid);
                    //check whether request has been sent to the socket.
                         if(newbuffer!=NULL)
                    {
                            for(int s=0;s<ask.nreq_grps;s++)  //group_number
                            {
                                 if(strncmp(ask.r_gp[s].grp_unid,newbuffer,6)==0)
                              {     gp_m=s;
                                 for(int i=0;i<4;i++)
                                 {
                                        if(ask.r_gp[s].clnt_sct[i]==socket)
                                        {
                                    //check whether grpid present in structure-iterate over the pending group
                                   
                                    
                                    ask.r_gp[s].no_of_reqs--;
                                   
                                    if(ask.r_gp[s].no_of_reqs==0) //form a group
                                     {
                                         int z=0;
                                       
                                         //transfer the details of all clients with admin to new group.
                                         while(ask.r_gp[s].total_clients!=0)
                                         {
                                            finalgrp.groups[finalgrp.nogps].clt_sockt[z++]=ask.r_gp[s].clnt_sct[z++];
                                            ask.r_gp[s].total_clients--;

                                         }
                                         
                                         strcpy(finalgrp.groups[finalgrp.nogps].grp_id,newbuffer);
                                         finalgrp.groups[finalgrp.nogps].grp_admin=ask.r_gp[s].req_admin;
                                         finalgrp.groups[finalgrp.nogps].clt_sockt[z++]=ask.r_gp[s].clnt_sct[z++];
                                         finalgrp.groups[finalgrp.nogps].broadcast=0;
                                         finalgrp.nogps++;
                                        
                                     }
                                     for(int i=0;i<4;i++)
                                     {
                                        sending_data_to_client(finalgrp.groups[gp_m].clt_sockt[i],infoadded); 
                                     }
                                }
                            }
                        }
                        
                  }}
                //   for(int i=0;i<4;i++)
                //   {
                //      sending_data_to_client(finalgrp.groups[gp_m].clt_sockt[i],infoadded);        
                //   }
                  return 0;}
             


        
    if(strncmp(newbuffer,"/makegroup",10) == 0)
       {
            char grp_join[200];
             // generating unique id for the group
            unsigned int seed=10000; 
            int grp_uniq_id;
            char info[250];
            
            
            srand(seed);
            
            grp_uniq_id=rand()%h;     //generate the random number
            h=h+2;
            char grp_key[5];
            char gr_k[5];
            int i=0;
            int clint_indx=0;
            sprintf(grp_key,"%d",grp_uniq_id);
            int j=0;
            
            //printf("here after sprintf \n");
            //sprintf(uni_id,"%d",uniq_id);
            // STORE THE GROUP DETAILS IN A STRUCTURE with the filedescriptors of group members.
              // char *first=strtok(buffer," ");   //makegroup
            // printf("here after first strtok %s\n",first);
                while(newbuffer!=NULL)
                {
                   
                    newbuffer=strtok(NULL," ");    // client id
                   
                    if(newbuffer!=NULL)

                    {    

                        clint_indx=search_clientIndexby_uniID(newbuffer);
                        makegp.groups[makegp.nogps].clt_sockt[i++]=serverside.client_list[clint_indx].filedes;
                     
                     }
                   
                }
                
                makegp.groups[makegp.nogps].clt_sockt[i++]=serverside.client_list[index_sender].filedes;
               
               
                makegp.groups[makegp.nogps].grp_admin=socket;
                 makegp.groups[makegp.nogps].broadcast=0;
                
                strcpy( makegp.groups[makegp.nogps].grp_id,grp_key);
                //strcpy(gp_info[ttl_grps].grp_id,grp_key);
                sprintf(info,"you are now admin of a group: GROUP ID: %s",grp_key);
                memset(grp_join,0,sizeof(grp_join));
        
                sprintf(grp_join,"You are added to a group: GROUP ID: %d",grp_uniq_id);
                int admin_skt=0;
                for(int k=0;k<i;k++)   // sending msg to all the clients that they have been added to a group
                {
                    // printf("in the for loop %d %d,%d\n",gp_info[ttl_grps].clt_sockt[k],i,k);
                   if( makegp.groups[makegp.nogps].clt_sockt[k]!=socket)
                    sending_data_to_client( makegp.groups[makegp.nogps].clt_sockt[k],grp_join); 
                       else
                       admin_skt= makegp.groups[makegp.nogps].clt_sockt[k];    
                }
                
                 sending_data_to_client(admin_skt,info);
                  makegp.nogps++; 
                        
       }
         }
       return 0;
}
// add new clients information to data structure

int server_add_new_client(struct sockaddr_in client_info, int new_socket_fd)
{
   // char ip[INET_ADDRSTRLEN]={0};
    //------------------------------------------------------------------------------------------
    unsigned int seed=10000;
    char welcme_msg[max_buffer]; 
    int uniq_id;
    
    srand(seed);
    // store the welcome message
    uniq_id=rand()%i;     //generate the random number
    i=i+2;
    //printf("in server add new client fxn---%s",nomore);
    char id[5];
    char uni_id[5];
    sprintf(id,"%d",uniq_id);
    //sprintf(uni_id,"%d",uniq_id);
     
    strcpy(welcme_msg,id);
    strcat(welcme_msg,":");
    strcat(welcme_msg,"WELCOME !");
   
    //-------------------------------------------------------------------------------------------
    char abuffer[max_buffer];
    //get extra server details
    receive_data_from_client(new_socket_fd,abuffer);   // to get the name of the client.
    //get the ip and port of client
    int port=ntohs(client_info.sin_port);
    if(serverside.all_clients>=max_clients)
    {
        char nomore[400];
     strcpy(nomore," :NO MORE REQUESTS CAN BE TAKEN");
     perror("Sorry!No more requests could be entertained.");
        sending_data_to_client(new_socket_fd,nomore);
        
         memset(nomore,0,sizeof(nomore));

        return -1;
        
    }
   // printf("[CLIENT-INFO] : [port = %d, ip=%s]\n",port,ip);
    printf("[CLIENT-INFO] : [port = %d]\n",port);
   //s memset(buffer,0,sizeof(buffer));
     sending_data_to_client(new_socket_fd,welcme_msg);
     memset(welcme_msg,0,sizeof(welcme_msg));
    //populating new client data
    strncpy(serverside.client_list[serverside.all_clients].client_name,abuffer,strlen(abuffer));
    serverside.client_list[serverside.all_clients].port=port;
    strcpy(serverside.client_list[serverside.all_clients].client_id,id);
    //strcpy(serverside.client_list[serverside.all_clients].ip,ip);
   
    serverside.client_list[serverside.all_clients].filedes=new_socket_fd;
    //printf("index of client in server-->%d\n",serverside.all_clients);
    //printf("file descriptors of clients present at server-->%d\n",serverside.client_list[serverside.all_clients].filedes);
    serverside.all_clients++;
    return 1;

}

// New client accept connection
int handling_new_client(int listen_fd, int *new_socket_fd)
{
    struct sockaddr_in client_addr;
    int len=sizeof(struct sockaddr);
    bzero(&client_addr,sizeof(client_addr));
    if((*new_socket_fd=accept(listen_fd,(struct sockaddr *)&client_addr,&len))<0)
    {
        perror("ACCEPT HAS FAILED");
        return -1;

    }
   int n= server_add_new_client(client_addr,*new_socket_fd);  //RUNNING TILL HERE.

                if(n==-1)
                    return -1;
    
    return 0;

}
int search_clientIndexby_socket(int socket) {
    int index = 0;
    for(int i = 0; i<serverside.all_clients; i++) {
           if(serverside.client_list[i].filedes == socket) {
                 index =i;
                return index;

           }
    }
    return index;
}
	
 	
 	

