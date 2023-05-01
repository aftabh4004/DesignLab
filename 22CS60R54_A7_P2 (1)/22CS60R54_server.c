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
#define CLIENT_LIMIT 10
#define MESSAGE_LEN 100
#define GROUP_LIMIT 5
#define GROUP_COUNT_LIMIT 10


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

typedef struct group_info {
	int id;
	int* admins;
	int isBroadcastOnly;
	int * members;
	int gsize;
	int asize;

} group_info;

typedef struct pending_group {
	int id;
	int no_req_pending;
	int* members;
	int* admins;
} pending_group;


client_table * ctab[CLIENT_LIMIT];
message_table * mtab[CLIENT_LIMIT];
group_info * gtab[GROUP_COUNT_LIMIT];
pending_group* pgtab[GROUP_COUNT_LIMIT];

int next_available_id = 10000;
int next_available_gid = 30000;

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
	for(int i = 0; i < GROUP_COUNT_LIMIT; i++) gtab[i] = NULL;
	for(int i = 0; i < GROUP_COUNT_LIMIT; i++) pgtab[i] = NULL;

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


/* function to processs messages from clients*/
void sendMsg(char * msg, int sockfd){
	char buffer[BUF_LEN];
	bzero(buffer, BUF_LEN);
	sprintf(buffer,"%s", msg);
	send(sockfd,buffer, sizeof(buffer), 0);
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



// Function to check if a client is online or not.
int isActive(int cid){
	for(int i = 0; i < CLIENT_LIMIT; i++)
		if (ctab[i] && ctab[i]->cid == cid) return 1;
	return 0;
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




//========================================Group functions ==========================================

// Function to check valid client id for group formation
int isGroupMemberValid(int gmembers[]){
	int* active =  printRecClient();
	int isDuplicate[CLIENT_LIMIT] = {0};
	for (int i = 0; i < GROUP_LIMIT; i++){
		int flag = 0;
		for (int j = 0; j < CLIENT_LIMIT; j++){
			if (active[j] != -1 && active[j] == gmembers[i]){
				flag = 1;
				if (isDuplicate[j]){
					return 0;
				}
				isDuplicate[j] = 1;
				break;
			}
		}
		if (flag == 0 && gmembers[i] != -1) return 0;
	}
	return 1;
}

int getFirstEmptyGroupSlot(){
	for (int i = 0; i < GROUP_COUNT_LIMIT; i++)
		if (gtab[i] == NULL) return i;
	return -1;
}

int getFirstEmptyPendingGroupSlot(){
	for (int i = 0; i < GROUP_COUNT_LIMIT; i++)
		if (pgtab[i] == NULL) return i;
	return -1;
}


int getFirstEmptyMemberSlot(int k){
	int *members = gtab[k]->members;

	for(int i = 0; i < GROUP_LIMIT; i++){
		if(members[i] == -1) return i;
	}
	return -1;
}


int addGroupInformation(int* gmembers, int* admins, int isBroadcastOnly, int sender, int gid){

	int k = getFirstEmptyGroupSlot();
	gtab[k] = (group_info*) malloc(sizeof(group_info));

	gtab[k]->id = gid;
	

	gtab[k]->admins = admins;
	gtab[k]->members = gmembers;
	gtab[k]->isBroadcastOnly = isBroadcastOnly;
	gtab[k]->gsize = 0;
	gtab[k]->asize = 1;

	for(int i = 0; i < GROUP_LIMIT; i++){
		if (gmembers[i] == -1) continue;
		if(sender == gmembers[i]) continue;

		char buffer[BUF_LEN];
		bzero(buffer, BUF_LEN);

		if (isBroadcastOnly) 
			sprintf(buffer, "[SERVER] You have been added to broadcast group %d\n", gtab[k]->id);
		else
			sprintf(buffer, "[SERVER] You have been added to group %d\n", gtab[k]->id);

		
		sendMsg(buffer, getClientsd(gmembers[i]));
		(gtab[k]->gsize)++;
	}
	return gtab[k]->id;
}

int isValidGroupId(int gid){
	for(int i = 0; i < GROUP_COUNT_LIMIT; i++){
		if (gtab[i] && gtab[i]->id == gid){
			return 1;
		}
	}
	return 0;
}

int getGroupIndexById(int gid){
	for(int i = 0; i < GROUP_COUNT_LIMIT; i++){
		if (gtab[i] && gtab[i]->id == gid){
			return i;
		}
	}
	return -1;
}

int getPendingGroupIndexById(int gid){
	for(int i = 0; i < GROUP_COUNT_LIMIT; i++){
		if (pgtab[i] && pgtab[i]->id == gid){
			return i;
		}
	}
	return -1;
}


int isMemberofGroup(int sender, int k){
	
	for (int i = 0; i < GROUP_LIMIT; i++){
		if(gtab[k]->members[i] == sender)
			return 1;

	}
	return 0;
}

int isAdmin(int member, int k){
	int* admins = gtab[k]->admins;

	for (int i = 0; i < GROUP_LIMIT; i++){
		if(admins[i] != -1 && admins[i] == member)
			return 1;
	}
	return 0;
}

int addPendingGroupInformation(int* gmembers, int* admins, int sender){

	int k = getFirstEmptyPendingGroupSlot();
	printf("after first index\n");
	pgtab[k] = (pending_group*) malloc(sizeof(pending_group));

	pgtab[k]->id = next_available_gid;
	next_available_gid++;

	pgtab[k]->admins = admins;
	pgtab[k]->members = gmembers;
	pgtab[k]->no_req_pending = 0;
	

	for(int i = 0; i < GROUP_LIMIT; i++){
		printf("inside loop\n");

		if (gmembers[i] == -1) continue;
		if(sender == gmembers[i]) continue;

		char buffer[BUF_LEN];
		bzero(buffer, BUF_LEN);
		sprintf(buffer, "[SERVER] You have been requested to join group %d use /joingroup <group_id> to join or /declinegroup <group_to> to decline the request.\n", pgtab[k]->id);
		
		sendMsg(buffer, getClientsd(gmembers[i]));
		(pgtab[k]->no_req_pending)++;
	}
	return pgtab[k]->id;
}


void sendGroupMessageHelper(int sender, int k, int sockfd, int gid, char* message){
	char buffer[BUF_LEN];
	bzero(buffer, BUF_LEN);

	for (int i = 0; i < GROUP_LIMIT; i++){
		if(gtab[k]->members[i] != sender){
			
			//sender ==  -1 when function is called to inform other member that a particular member is being removed.
			if(sockfd == -1)
				sprintf(buffer,"%s", message);
			else
				sprintf(buffer, "[%d in Group %d] %s\n", sender, gid, message);
			sendMsg(buffer, getClientsd(gtab[k]->members[i]));
		}

	}
	if(sockfd != -1){
		sprintf(buffer, "[SERVER] Message Sent\n");
		sendMsg(buffer, sockfd);
	}
}

int sendGroupMsg(char * message, int gid, int sockfd){
	int sender = getClientId(sockfd);
	int k = getGroupIndexById(gid);
	int isBCG = gtab[k]->isBroadcastOnly;

	char buffer[BUF_LEN];
	bzero(buffer, BUF_LEN);

	if (isMemberofGroup(sender, k)){

		if (isBCG){
			if (isAdmin(sender, k)){
				sendGroupMessageHelper(sender, k, sockfd, gid, message);
			}
			else{
				sprintf(buffer, "[SERVER] Only Admin can message in Group %d\n", gid);
				sendMsg(buffer, sockfd);
			}
		}
		else{
			sendGroupMessageHelper(sender, k, sockfd, gid, message);
		}
		
	}
	else{
		sprintf(buffer, "[SERVER] You are not a member of Group %d\n", gid);
		sendMsg(buffer, sockfd);
	}



}

void removeMember(int cid, int k){
	int * members = gtab[k]->members;
	for(int i = 0; i < GROUP_LIMIT; i++) 
		if (members[i] == cid){
			members[i] = -1;
			(gtab[k]->gsize)--;
			return;
		}
}

void removeAdmin(int cid, int k){
	int * admins = gtab[k]->admins;
	for(int i = 0; i < GROUP_LIMIT; i++) 
		if (admins[i] == cid){
			admins[i] = -1;
			(gtab[k]->asize)--;
			return;
		}
}

// Function to delete the group and infom the members.
void deleteGroup(int k){
	char buffer[BUF_LEN];
	int* members = gtab[k]->members;

	for (int i = 0; i < GROUP_LIMIT; i++){
		if (members[i] != -1){
			bzero(buffer, BUF_LEN);
			sprintf(buffer, "[SERVER] Group %d has been deleted\n", gtab[k]->id);
			sendMsg(buffer, getClientsd(members[i]));
		}
	}

	free(members);
	free(gtab[k]->admins);
	free(gtab[k]);
	gtab[k] = NULL;
}

void removefromallGroup(int cid){
	char buffer[BUF_LEN];
	for(int i = 0; i < GROUP_COUNT_LIMIT; i++){
		if(gtab[i] && isMemberofGroup(cid, i)){
			int gid = gtab[i]->id;
			if(isAdmin(cid, i))
				removeAdmin(cid, i);
			removeMember(cid, i);
			
			//tell everyone the cid left
			bzero(buffer, BUF_LEN);
			sprintf(buffer, "[SERVER] %d left the group %d\n", cid, gid);
			sendGroupMessageHelper(-1, i, -1, gid, buffer);
			

			bzero(buffer, BUF_LEN);
			sprintf(buffer, "[SERVER] You have been removed from the Group %d\n", gtab[i]->id);
			sendMsg(buffer, getClientsd(cid));

			if(gtab[i]->asize <= 0){
				deleteGroup(i);
			}
		}
	}
}

// ======================================= Handlers =============================================================
// ==============================================================================================================


void handle_active(int sockfd){
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


void handle_sendgroup(int sockfd){
	char * group_id = strtok(NULL, " ");
	int gid;
	if (group_id == NULL){
		sendMsg("[SERVER] Invalid format, please follow /sendgroup <group id> <Message>\n", sockfd);
		return ;
	}
	gid = atoi(group_id);
	if (gid < 0){
		sendMsg("[SERVER] Invalid Group ID, Group ID must be a 5 digit integer\n", sockfd);
		return ;
	}

	char *message = strtok(NULL, "\n");
	if (message == NULL){
		sendMsg("[SERVER] Empty message!\n", sockfd);
		return ;
	}

	if (isValidGroupId(gid)){
		if (sendGroupMsg(message, gid, sockfd))
			sendMsg("[SERVER] Message Sent\n", sockfd);
	}
	else{
		sendMsg("[SERVER] No such group found.\n", sockfd);
	}
}

void handle_send(int sockfd){
	char * client_id = strtok(NULL, " ");
	int cid;
	if (client_id == NULL){
		sendMsg("[SERVER] Invalid format, please follow /send <client id> <Message>\n", sockfd);
		return ;
	}
	cid = atoi(client_id);
	if (cid < 0){
		sendMsg("[SERVER] Invalid Client ID, Client ID must be a 5 digit integer\n", sockfd);
		return ;
	}

	char *message = strtok(NULL, "\n");
	if (message == NULL){
		sendMsg("[SERVER] Empty message!\n", sockfd);
		return ;
	}

	if (isActive(cid)){
		Message* msg = (Message*) malloc(sizeof(Message));
		strcpy(msg->text, message);
		msg->sender = getClientId(sockfd);
		msg->next = NULL;
		addMsgEntry(msg, cid);
		sendMsg("[SERVER] Message Sent\n", sockfd);
	}
	else{
		sendMsg("[SERVER] The recipient might have left the room :( or the Client_id is invalid.\n", sockfd);
	}
}





void handle_makegroup(int sockfd, int isBroadcastOnly){
	int *gmembers = (int*) malloc(sizeof(int) * GROUP_LIMIT), k = 0;
	int * admins = (int*) malloc(sizeof(int) * GROUP_LIMIT);
	int sender = getClientId(sockfd);

	//Initialize
	for (int i = 0; i < GROUP_LIMIT; i++) {
		gmembers[i] = -1;
		admins[i] = -1;
	}

	gmembers[k++] = sender;
	admins[0] = getClientId(sockfd);
	char * token = strtok(NULL, " ");

	if(token == NULL){
		sendMsg("[SERVER] Need atleast one client.\n", sockfd);
		return;
	}
	while(token && k < GROUP_LIMIT){
		int member = atoi(token);
		if (member != sender) gmembers[k++] = member;
		token = strtok(NULL, " ");
	}

	if (token){
		sendMsg("[SERVER] Group size must be less than 5 including you.\n", sockfd);
		return;
	}


	if(isGroupMemberValid(gmembers)){
		int gid = next_available_gid;
		next_available_gid++;
		addGroupInformation(gmembers, admins, isBroadcastOnly, sender, gid);
		printf("Group formed Successfully. group id : %d\n", gid);
		char buffer[BUF_LEN];
		bzero(buffer, BUF_LEN);
		sprintf(buffer, "[SERVER] Group formed Successfully. Group ID: %d\n", gid);
		sendMsg(buffer, sockfd);

	}else{
		printf("Some client id found to be invalid in group formation.\n");
		sendMsg("[SERVER] Some client id might be invalid\n", sockfd);

	}
}


void handle_makegroupbroadcast(int sockfd){
	char * token = strtok(NULL, " ");

	if(token == NULL){
		sendMsg("[SERVER] Invalid format.\n", sockfd);
		return;
	}

	int gid = atoi(token);
	int sender = getClientId(sockfd);

	if(!isValidGroupId(gid)){
		sendMsg("[SERVER] Invalid Group ID.\n", sockfd);
		return;
	}

	int k = getGroupIndexById(gid);

	if(!isAdmin(sender, k)){
		sendMsg("[SERVER] Only Admins can execute this command.\n", sockfd);
		return;
	}

	gtab[k]->isBroadcastOnly = 1;

	char buffer[BUF_LEN];
	bzero(buffer, BUF_LEN);
	sprintf(buffer, "[SERVER] Group %d is modified as broadcast only.\n", gid);
	sendGroupMessageHelper(-1, k, -1, gid, buffer);

}

void handle_activegroup(int sockfd){
	int flag = 0;
	char active_group[BUF_LEN], temp[7];
	int sender =  getClientId(sockfd);
	bzero(active_group, BUF_LEN);

	for (int i = 0; i < GROUP_COUNT_LIMIT; i++){
		bzero(active_group, BUF_LEN);

		if (gtab[i] && isMemberofGroup(sender, i)){
			flag = 1;
			bzero(temp, 7);
			sprintf(temp, "%d\t", gtab[i]->id);
			strcat(active_group, temp);
			
			int* members = gtab[i]->members;
			for (int j = 0; j < GROUP_LIMIT; j++){
				if (members[j] == -1) continue;
				
				bzero(temp, 7);
				sprintf(temp, " %d", members[j]);
				strcat(active_group, temp);
				if(isAdmin(members[j], i))
					strcat(active_group, "(Admin)");
				if (members[j] == sender)
					strcat(active_group, "(You)");
				
			}
			if(gtab[i]->isBroadcastOnly)
				strcat(active_group, " Broadcast");

			strcat(active_group, "\n");
			sendMsg(active_group, sockfd);
		}
	}

	if (flag == 0) sendMsg("[SERVER] You are not a part of any group.\n", sockfd);
}

void handle_makegroupadmin(int sockfd){
	char* token = strtok(NULL, " ");
	if(token == NULL){
		sendMsg("[SERVER] Invalid format.\n", sockfd);
		return;
	}

	int gid = atoi(token);
	
	token = strtok(NULL, " ");
	if(token == NULL){
		sendMsg("[SERVER] Invalid format.\n", sockfd);
		return;
	}

	int cid = atoi(token);
	int sender = getClientId(sockfd);
	int k = getGroupIndexById(gid);

	if(k == -1){
		sendMsg("[SERVER] Invalid Group ID.\n", sockfd);
		return;
	}
	
	if(!isAdmin(sender, k)){
		sendMsg("[SERVER] Only admins can execute this command.\n", sockfd);
		return;
	}

	if(!isMemberofGroup(cid, k)){
		sendMsg("[SERVER] This member is not a part of group.\n", sockfd);
		return;
	}

	if(isAdmin(cid, k)){
		sendMsg("[SERVER] Member is alreay an admin.\n", sockfd);
		return;
	}

	gtab[k]->admins[gtab[k]->asize] = cid;
	(gtab[k]->asize)++;
	

	printf("[SERVER] %d is now an admin of Group %d.\n", cid, gid);
	sendMsg("[SERVER] Member is now an admin.\n", sockfd);

	char buffer[BUF_LEN];
	bzero(buffer, BUF_LEN);
	sprintf(buffer, "[SERVER] You are now an admin of Group %d\n", gid);
	sendMsg(buffer, getClientsd(cid));
	return;

}

void handle_addtogroup(int sockfd){
	int sender = getClientId(sockfd);
	char * token = strtok(NULL, " "), buffer[BUF_LEN];
	if(token == NULL){
		sendMsg("[SERVER] Invalid format\n", sockfd);
		return;
	}
	int gid = atoi(token);
	if(!isValidGroupId(gid)){
		sendMsg("[SERVER] Invalid Group ID\n", sockfd);
		return;
	}
	int k = getGroupIndexById(gid);

	if (!isAdmin(sender, k)){
		sendMsg("[SERVER] Only admins can add member to group\n", sockfd);
		return;
	}
	token = strtok(NULL, " ");

	if(token == NULL){
		sendMsg("[SERVER] Need atleast one client ID\n", sockfd);
		return;
	} 
	while(token){

		if (gtab[k]->gsize >= GROUP_LIMIT){
			sendMsg("[SERVER] Unable to add more member, Participents limit exceeded\n", sockfd);
			return;
		}


		int cid  = atoi(token);
		if (!isActive(cid)){
			bzero(buffer, BUF_LEN);
			sprintf(buffer, "[SERVER] Invalid client id %s\n", token);
			sendMsg(buffer, sockfd);
			token = strtok(NULL, " ");
			continue;
		}

		if (isMemberofGroup(cid, k)){
			bzero(buffer, BUF_LEN);
			sprintf(buffer, "[SERVER] %d already a member of %d\n", cid, gid);
			sendMsg(buffer, sockfd);
			token = strtok(NULL, " ");
			continue;
		}
		
		int m = getFirstEmptyMemberSlot(k);
		gtab[k]->members[m] = cid;
		(gtab[k]->gsize)++;
		bzero(buffer, BUF_LEN);
		sprintf(buffer, "[SERVER] You are added to Group %d by %d\n", gid, sender);
		sendMsg(buffer, getClientsd(cid));


		bzero(buffer, BUF_LEN);
		sprintf(buffer, "[SERVER] %d added to Group %d\n", cid, gid);
		sendMsg(buffer, sockfd);

		token = strtok(NULL, " ");
	}
}


void handle_removefromgroup(int sockfd){
	int sender = getClientId(sockfd);
	char * token = strtok(NULL, " "), buffer[BUF_LEN];
	if(token == NULL){
		sendMsg("[SERVER] Invalid format\n", sockfd);
		return;
	}
	int gid = atoi(token);
	if(!isValidGroupId(gid)){
		sendMsg("[SERVER] Invalid Group ID\n", sockfd);
		return;
	}
	int k = getGroupIndexById(gid);

	if (!isAdmin(sender, k)){
		sendMsg("[SERVER] Only admins can add member to group\n", sockfd);
		return;
	}
	token = strtok(NULL, " ");

	if(token == NULL){
		sendMsg("[SERVER] Need atleast one client ID\n", sockfd);
		return;
	} 
	while(token){


		int cid  = atoi(token);
		
		if (sender == cid){
			sendMsg("[SERVER] You can't remove yourself. LOL.\n", sockfd);
			return;
		}
		
		
		if (!isActive(cid)){
			bzero(buffer, BUF_LEN);
			sprintf(buffer, "[SERVER] Invalid client id %s\n", token);
			sendMsg(buffer, sockfd);
			token = strtok(NULL, " ");
			continue;
		}

		if (!isMemberofGroup(cid, k)){
			bzero(buffer, BUF_LEN);
			sprintf(buffer, "[SERVER] %d is not a member of %d\n", cid, gid);
			sendMsg(buffer, sockfd);
			token = strtok(NULL, " ");
			continue;
		}
		

		if(isAdmin(cid, k))
			removeAdmin(cid, k);
		removeMember(cid, k);

		//tell everyone the cid left
		bzero(buffer, BUF_LEN);
		sprintf(buffer, "[SERVER] %d have been removed from group %d by %d\n", cid, gid, sender);
		sendGroupMessageHelper(sender, k, -1, gid, buffer);
		
		bzero(buffer, BUF_LEN);
		sprintf(buffer, "[SERVER] You are removed from Group %d by %d\n", gid, sender);
		sendMsg(buffer, getClientsd(cid));


		bzero(buffer, BUF_LEN);
		sprintf(buffer, "[SERVER] %d removed from Group %d\n", cid, gid);
		sendMsg(buffer, sockfd);

		token = strtok(NULL, " ");
	}
}

void handle_makegroupreq(int sockfd){
	int *gmembers = (int*) malloc(sizeof(int) * GROUP_LIMIT), k = 0;
	int * admins = (int*) malloc(sizeof(int) * GROUP_LIMIT);
	int sender = getClientId(sockfd);

	//Initialize
	for (int i = 0; i < GROUP_LIMIT; i++) {
		gmembers[i] = -1;
		admins[i] = -1;
	}

	gmembers[k++] = sender;
	admins[0] = getClientId(sockfd);
	char * token = strtok(NULL, " ");
	while(token && k < GROUP_LIMIT){
		int member = atoi(token);
		if (member != sender) gmembers[k++] = member;
		token = strtok(NULL, " ");
	}

	if (token){
		sendMsg("[SERVER] Group size must be less than 5 including you.\n", sockfd);
		return;
	}


	if(isGroupMemberValid(gmembers)){
		
		int gid = addPendingGroupInformation(gmembers, admins, sender);
		printf("Group requested. group id : %d\n", gid);
		char buffer[BUF_LEN];
		bzero(buffer, BUF_LEN);
		sprintf(buffer, "[SERVER] Group request registred Successfully. Group ID: %d\n", gid);
		sendMsg(buffer, sockfd);

	}else{
		printf("Some client id found to be invalid in group formation.\n");
		sendMsg("[SERVER] Some client id might be invalid\n", sockfd);

	}
}

void handle_groupres(int sockfd, int join){
	char* token = strtok(NULL, " ");
	int gid = atoi(token);
	int sender = getClientId(sockfd);
	char buffer[BUF_LEN];
	bzero(buffer, BUF_LEN);

	if(join) sprintf(buffer, "[SERVER] Your request has been processed, You'll be added to the group soon.\n");
	else sprintf(buffer, "[SERVER] Your request has been processed.\n");

	int k = getPendingGroupIndexById(gid);

	if (k < 0){
		sendMsg("[SERVER] Invalid Group ID\n", sockfd);
		return;
	}

	int* members = pgtab[k]->members;
	int flag = 0;
	for(int j = 0; j < GROUP_LIMIT; j++){
		if (members[j] == -1) continue;

		if(members[j] == sender){
			flag = 1;
			if(!join) members[j] = -1;
			sendMsg(buffer, sockfd);

			(pgtab[k]->no_req_pending)--;

			if(pgtab[k]->no_req_pending <= 0){
				addGroupInformation(members, pgtab[k]->admins, 0, -1, pgtab[k]->id);
				free(pgtab[k]);
				pgtab[k] = NULL;
			}
			break;
		}
	}

	if (flag == 0) sendMsg("[SERVER] No Pending join request found for the provided group ID.\n", sockfd);
	
}

// Function to handle quit, remove client entry from client table and his messages from message table.
void handle_quit(int sockfd, int silent){
	int cid = getClientId(sockfd);
	removefromallGroup(cid);
	removeMsgEntry(cid);
	removeRecClient(cid);
	char buf[BUF_LEN];
	bzero(buf, BUF_LEN);
	
	

	if (!silent){
		sprintf(buf, "%d have left the chat room.\n", cid);
		printf("%d have left the chat room.\n", cid);    // Log on server
		broadcast(buf, 0, 0);
	} 
		
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


// =====================================================================================================
// =======================================================================================================

/*Function to handle all the commands as entered by the client*/ 
int performAction(int sockfd, char* buffer){
	
	char *cmd = NULL, *client_id = NULL, *message = NULL;
	cmd = strtok(buffer, " ");

	if (cmd == NULL){
		sendMsg("[SERVER] Invalid command.\n", sockfd);
		return 0;
	}

	if (strncmp(cmd, "/activegroup", 12) == 0)
		handle_activegroup(sockfd);

	else if (strncmp(cmd, "/active", 7) == 0)
		handle_active(sockfd);
	
	else if(strncmp(cmd, "/sendgroup", 10) == 0)
		handle_sendgroup(sockfd);
	
	else if (strncmp(cmd, "/send", 5) == 0)
		handle_send(sockfd);
		
	else if (strncmp(cmd, "/broadcast", 9) == 0){
		message = strtok(NULL, "\n");
		if (message == NULL){
			sendMsg("[SERVER] Empty message.\n", sockfd);
			return 0;
		}
		broadcast(message, getClientId(sockfd), 0);
		sendMsg("[SERVER] Message Sent.\n", sockfd);
	}

	else if (strncmp(cmd, "/makeadmin", 10) == 0)
		handle_makegroupadmin(sockfd);
	
	else if (strncmp(cmd, "/makegroupreq", 15) == 0)
		handle_makegroupreq(sockfd);

	else if (strncmp(cmd, "/joingroup", 11) == 0)
		handle_groupres(sockfd, 1);

	else if (strncmp(cmd, "/declinegroup", 15) == 0)
		handle_groupres(sockfd, 0);

	else if (strncmp(cmd, "/makegroupbroadcast", 19) == 0)
		handle_makegroupbroadcast(sockfd);

	else if (strncmp(cmd, "/makegroup", 10) == 0)
		handle_makegroup(sockfd, 0);
	
	else if (strncmp(cmd, "/addtogroup", 11) == 0)
		handle_addtogroup(sockfd);

	else if (strncmp(cmd, "/removefromgroup", 18) == 0)
		handle_removefromgroup(sockfd);

	else if (strncmp(cmd, "/quit", 5) == 0){
		handle_quit(sockfd, 0);
		return 1;
	}
	else{
		sendMsg("Invalid Command.\n", sockfd);
	}
	
	return 0;
	
}



// ============================      ======================================================================
// ============================ main ======================================================================
// ============================      ======================================================================

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

	int sockfd, newsockfd, portno, clilen, pid,client_id,flags, max_sock;
	struct sockaddr_in serv_addr, cli_addr;
	  
	sockfd = socket(AF_INET, SOCK_STREAM, 0);  /*getting a sockfd for a TCP connection*/
	max_sock = sockfd;
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
		int activity = select(FD_SETSIZE, &cur_fds, NULL, NULL, NULL); //give appropriate parameters 
		
		if((activity < 0)&&(errno!= EWOULDBLOCK)) //fill appropriate parameters here
		{
			error("!!ERROR ON SELECT!!");
		}


		
		for (int i = 0; i <= max_sock; i++){
			if (FD_ISSET(i, &cur_fds)){
				if (i ==  sockfd){
					newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
					if (newsockfd != -1){
						max_sock = newsockfd;
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

