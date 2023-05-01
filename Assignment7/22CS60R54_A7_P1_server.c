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
#include <sys/sem.h>	/* Include this to use semaphores */
#include <time.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <errno.h>

#define BUF_LEN 256
#define CLIENT_LIMIT 5
#define MESSAGE_LEN 100


typedef struct client_table {
	int sockfd;
	int cid;
} client_table;

typedef struct Message {
	int sender;
	char text[MESSAGE_LEN];
	struct Message * next;
} Message;

typedef struct message_table {
	int client_id;
	Message * msgList;
} message_table;

client_table * ctab[CLIENT_LIMIT];
message_table * mtab[CLIENT_LIMIT];
int next_available_id = 10000;
/*Define all the desired data structures as asked in the question*/


/*Function to handle errors*/
void error(char *msg)
{
	perror(msg);
	exit(0);
}

// /*Some utility functions to take care of client-server connections*/

void initializeRecEntry(){
	for(int i = 0; i < CLIENT_LIMIT; i++) ctab[i] = NULL;
	for(int i = 0; i < CLIENT_LIMIT; i++) mtab[i] = NULL;
}

/*function to get the first empt index from Shared Memory Segment for storing list of active clients*/
int getEmptyClient(){
	int i = 0;
	while(i < CLIENT_LIMIT && ctab[i]) i++;

	if (i >= CLIENT_LIMIT) return -1;
	return i;
} 

/*function to add a new client entry to Shared Memory Segment for storing list of active clients*/
int addRecClient(int i, int sockfd){
	ctab[i] = (client_table*)malloc(sizeof(client_table));
	ctab[i]->sockfd = sockfd;
	ctab[i]->cid = next_available_id;
	next_available_id++;
	return ctab[i]->cid;
}

/*function to remove a disconnected client entry from Shared Memory Segment for storing list of active clients*/
void removeRecClient(int cid){
	for(int i = 0; i < CLIENT_LIMIT; i++){
		if(ctab[i] && ctab[i]->cid == cid){
			free(ctab[i]);
			ctab[i] = NULL;
			return;
		}
	}
}

/*function to print the contents of Shared Memory Segment for storing list of active clients*/
int* printRecClient(){
	int* active = (int*) malloc(sizeof(int) * CLIENT_LIMIT);
	for (int i = 0; i < CLIENT_LIMIT; i++){
		if (ctab[i]){
			active[i] = ctab[i]->cid;
		}
		else
			active[i] = -1;
	}
	return active;
}

/*function to get sockfd mapped to the client_id in Shared Memory Segment for storing list of active clients*/
int getClientsd(int cid){
	for(int i = 0; i < CLIENT_LIMIT; i++){
		if(ctab[i] && ctab[i]->cid == cid) return ctab[i]->sockfd;
	}
	return -1;
}

/*function to get unique client id mapped to sockfd from the Shared Memory Segment for storing list of active clients*/
int getClientId(int sockfd){
	for(int i = 0; i < 10; i++){
		if(ctab[i] && ctab[i]->sockfd == sockfd)
			return ctab[i]->cid;
	}
	return -1;
}


// /*FUNCTIONS BELOW HANLES ALL SORT OF MESSAGE QUERIES FROM CLIENT*/

/* function to initializeShared Memory Segment for storing list of messages from clients*/
void initializeMsgEntry(int id){
	int i = 0;
	while(i < CLIENT_LIMIT && mtab[i]) i++;
	mtab[i] = (message_table*) malloc(sizeof(message_table));
	mtab[i]->client_id = id;
	mtab[i]->msgList = NULL;
}

 /* function to get the first empty location form  Shared Memory Segment for storing list of messages from clients*/
int getMsgEntryFromID(int id){
	for(int i = 0; i < CLIENT_LIMIT; i++)
		if (mtab[i] && mtab[i]->client_id == id) return i;
	return -1;	
}

/* function to add a new msg to the Shared Memory Segment for storing list of messages from clients*/
void addMsgEntry(Message* msg, int id){
	int i = getMsgEntryFromID(id);

	if(i == -1) return;
	Message * head = mtab[i]->msgList, *cur;
	cur = head;
	if (head == NULL) {
		mtab[i]->msgList = msg;
		return;
	}
	
	while(cur && cur->next) cur = cur->next;

	cur->next = msg;

}

/* function to remove an entry from Shared Memory Segment for storing list of messages from clients*/
void removeMsgEntry(int cid){
	for (int i = 0; i < CLIENT_LIMIT; i++){
		if (mtab[i] && mtab[i]->client_id == cid){
			Message* head = mtab[i]->msgList, *temp;
			while (head){
				temp = head;
				head = head->next;
				free(temp);
			}
			mtab[i]->msgList = NULL;
			free(mtab[i]);
			mtab[i] = NULL;
			return;
		}
	}
}

/* function to processs messages from clients*/
void sendMsg(char * msg, int sockfd){
	char buffer[BUF_LEN];
	bzero(buffer, BUF_LEN);
	sprintf(buffer,"%s", msg);
	send(sockfd,buffer, sizeof(buffer), 0);
}

/*FUCTION TO HANDLE BROADCAST REQUEST*/
void broadcast(char * message, int sender, int not_send_to){
	int* active = printRecClient();

	for (int i = 0; i < CLIENT_LIMIT; i++){
		if(active[i] != -1 && active[i] != sender && active[i] != not_send_to) {
			Message* msg = (Message*) malloc(sizeof(Message));
			strcpy(msg->text, message);
			msg->sender = sender;
			msg->next = NULL;
			addMsgEntry(msg, active[i]);

		}
	}
	
}

// Function to check if a client is online or not.
int isActive(int cid){
	for(int i = 0; i < CLIENT_LIMIT; i++)
		if (ctab[i] && ctab[i]->cid == cid) return 1;
	return 0;
}


// Function to handle quit, remove client entry from client table and his messages from message table.
void handle_quit(int sockfd, int silent){
	int cid = getClientId(sockfd);
	removeMsgEntry(cid);
	removeRecClient(cid);
	char buf[BUF_LEN];
	bzero(buf, BUF_LEN);
	sprintf(buf, "%d have left the chat room.\n", cid);
	printf("%d have left the chat room.\n", cid);    // Log on server

	if (!silent) broadcast(buf, 0, 0);
	sendMsg("STOP", sockfd);
	close(sockfd);
}


/*Function to handle signals ^C, ^D, ^Z*/
void sigHandler() 
{ 

	int* active =  printRecClient();
	for (int i = 0; i < CLIENT_LIMIT; i++){
		if(active[i] != -1){
			int sockfd = getClientsd(active[i]); 
			sendMsg("[Server] Server is going offline!.\n", sockfd);
			handle_quit(sockfd, 0);
		} 
	}
	printf("\nExiting Server...\n");
	exit(0);
} 


/*Function to handle all the commands as entered by the client*/ 
int performAction(int sockfd, char* buffer){
	
	char *cmd = NULL, *client_id = NULL, *message = NULL;
	cmd = strtok(buffer, " ");

	if (cmd == NULL){
		sendMsg("[SERVER] Invalid command.\n", sockfd);
		return 0;
	}
	if (strncmp(cmd, "/active", 7) == 0){
		char active_now[BUF_LEN];
		bzero(active_now, BUF_LEN);
		int * active = printRecClient();

		int cid = getClientId(sockfd);

		for (int i = 0; i < CLIENT_LIMIT; i++)
			if(active[i] != -1){
				if (cid == active[i]){
					char temp[13];
					bzero(temp, 13);
					sprintf(temp, "%d (You)\n", active[i]);
					strncat(active_now, temp, 13);
				}
				else{
					char temp[7];
					bzero(temp, 7);
					sprintf(temp, "%d\n", active[i]);
					strncat(active_now, temp, 6);
				}

			}
		

		sendMsg(active_now, sockfd);
	}
	else if (strncmp(cmd, "/send", 5) == 0){
		client_id = strtok(NULL, " ");
		int cid;
		if (client_id == NULL){
			sendMsg("[SERVER] Invalid format, please follow /send <client id> <Message>\n", sockfd);
			return 0;
		}
		cid = atoi(client_id);
		if (cid < 0){
			sendMsg("[SERVER] Invalid Client ID, Client ID must be a 5 digit integer\n", sockfd);
			return 0;
		}

		message = strtok(NULL, "\n");
		if (message == NULL){
			sendMsg("[SERVER] Empty message!\n", sockfd);
			return 0;
		}

		if (isActive(cid)){
			Message* msg = (Message*) malloc(sizeof(Message));
			strcpy(msg->text, message);
			msg->sender = getClientId(sockfd);
			msg->next = NULL;
			addMsgEntry(msg, cid);
		}
		else{
			sendMsg("[SERVER] The recipient might have left the room :( or the Client_id is invalid.\n", sockfd);
		}
		
	}
	else if (strncmp(cmd, "/broadcast", 9) == 0){
		message = strtok(NULL, "\n");
		if (message == NULL){
			sendMsg("[SERVER] Empty message.\n", sockfd);
			return 0;
		}
		broadcast(message, getClientId(sockfd), 0);
	}
	else if (strncmp(cmd, "/quit", 5) == 0){
		handle_quit(sockfd, 0);
		return 1;
	}
	else{
		sendMsg("Yet to implement.\n", sockfd);
	}
	
	return 0;
	
}


int main(int argc, char *argv[])
{
	srand(time(0)); /*seeding the rand() function*/
	
	signal(SIGINT, sigHandler);  // handles ^C
	signal(SIGTSTP, sigHandler);    //handles ^Z
	signal(SIGQUIT, sigHandler);    //handles ^D
	
	
	if (argc < 2) {
		fprintf(stderr,"ERROR, no port provided\n");
		exit(1);
	}

	int sockfd, newsockfd, portno, clilen, pid,client_id,flags;
	struct sockaddr_in serv_addr, cli_addr;
	  
	sockfd = socket(AF_INET, SOCK_STREAM, 0);  /*getting a sockfd for a TCP connection*/
	if (sockfd < 0)  perror("ERROR opening socket");

	int optval = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,  &optval, sizeof(optval));

	if ((flags = fcntl(sockfd, F_GETFL, 0)) < 0) { 
		error("can't get flags to SCOKET!!");
	} 


	if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) < 0) { 
		error("fcntl failed.");
	}

	bzero((char *) &serv_addr, sizeof(serv_addr));

	portno = atoi(argv[1]);
	serv_addr.sin_family = AF_INET; /*symbolic constant for IPv4 address*/
	serv_addr.sin_addr.s_addr = INADDR_ANY; /*symbolic constant for holding IP address of the server*/
	serv_addr.sin_port = htons(portno);

	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
		error("ERROR on binding");
 	}
	int ret_val = listen(sockfd, 5);
	if (ret_val != 0) {
		fprintf(stderr, "listen failed [%s]\n", strerror(errno));
		close(sockfd);
		return -1;
	}
 	/*Initialize all the data structures and the semaphores you will need for handling the client requests*/
 	fd_set fds, cur_fds;

	FD_ZERO(&fds);
	FD_SET(sockfd, &fds);
 	initializeRecEntry();
	

	while (1) {
		cur_fds = fds;
		int activity = select(10, &cur_fds, NULL, NULL, NULL); //give appropriate parameters 
		
		if((activity < 0)&&(errno!= EWOULDBLOCK)) //fill appropriate parameters here
		{
			error("!!ERROR ON SELECT!!");
		}


		
		for (int i = 0; i < 10; i++){
			if (FD_ISSET(i, &cur_fds)){
				if (i ==  sockfd){
					newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
					if (newsockfd != -1){
						int freeloc = getEmptyClient();
						if (freeloc == -1){
							sendMsg("[SERVER] Can't connect at the moment, Connection Limit exceeded! Please try again.\n", newsockfd);
							printf("Unsuccessful connection due to Connection Limit exceeded!\n");   // Log on servver
							handle_quit(newsockfd, 1);
						}
						else{
							int id = addRecClient(freeloc, newsockfd);
							initializeMsgEntry(id);
							char buf[BUF_LEN];
							sprintf(buf, "[SERVER] Welcome, %d, use /active to see who's online!\n", id);
							sendMsg(buf, newsockfd);

							FD_SET(newsockfd, &fds);
							printf("New Connection accepted! on socketfd %d, client id %d\n", newsockfd, id);
							
							bzero(buf, BUF_LEN);
							sprintf(buf, "%d have joined the chat room, Say Hi to %d!.", id, id);
							broadcast(buf, 0, id);
						}
						

					}	
				}else{
					char buffer[BUF_LEN];
					bzero(buffer, BUF_LEN);
					recv(i, buffer, sizeof(buffer), 0);
					int id = getClientId(i);
					if(performAction(i, buffer))
						FD_CLR(i, &fds);
					
				}	
			}
		}



		// Sending all pending message in message table

		for(int i = 0; i < CLIENT_LIMIT; i++){
			if(mtab[i] == NULL) continue;

			int sfd = getClientsd(mtab[i]->client_id);
			Message* head = mtab[i]->msgList;
			
			Message* cur = head, *temp;
			char buffer[BUF_LEN];
			while(cur){
				bzero(buffer, BUF_LEN);
				if (cur->sender == 0) {
					sprintf(buffer, "[SERVER] %s\n", cur->text);
					printf("[SERVER -> %d] %s\n", mtab[i]->client_id, cur->text);   // Log on server
				}
				else{
					sprintf(buffer, "[%d] %s\n", cur->sender,  cur->text);
					printf("[%d -> %d] %s\n",cur->sender, mtab[i]->client_id, cur->text);   // Log on server
				}


				sendMsg(buffer, sfd);
				
				temp = cur;
				cur = cur->next;
				free(temp);
			}
			mtab[i]->msgList = NULL;

		}

		

	} /* end of while */
	
	return 0; /* we never get here */
}

