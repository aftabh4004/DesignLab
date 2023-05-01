#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <ctype.h>
#include <stdbool.h>
#define LINELEN 256
#define EXTRA 50
void error(char *msg){
    perror(msg);
    exit(0);
}

typedef struct record{
    char* roll;
    char* name;
    int marks;
} record;


void merge(char** data, char** feild_values_name, double* feild_values_int,  int start_row, int end_row, int mid){
    
    
    char ** temp = (char**) malloc(sizeof(char *)*(end_row - start_row + 1));
    char ** temp_fv_name = (char**) malloc(sizeof(char *)*(end_row - start_row + 1));
    int temp_fv[end_row - start_row + 1];
    int i = start_row, j = mid + 1;
    int k = 0;
    
    
    while(i <= mid && j <= end_row){
        
        int cond =  feild_values_int[i] < feild_values_int[j];
        if(feild_values_name) cond = strcmp(feild_values_name[i], feild_values_name[j]) < 0;
        
        if (cond){
            temp[k] = data[i];
            if (feild_values_name) temp_fv_name[k] = feild_values_name[i];
            else temp_fv[k] = feild_values_int[i];
            i++;k++;
        } 
        else {
            temp[k] = data[j];
            if (feild_values_name) temp_fv_name[k] = feild_values_name[j];
            else temp_fv[k] = feild_values_int[j];
            j++;k++;
        } 
    }

    while(i <= mid) {
        temp[k] = data[i];
        if (feild_values_name) temp_fv_name[k] = feild_values_name[i];
        else temp_fv[k] = feild_values_int[i];
        i++;k++;
    }

    while(j <= end_row){
        temp[k] = data[j];
        if (feild_values_name) temp_fv_name[k] = feild_values_name[j];
        else temp_fv[k] = feild_values_int[j];
        j++;k++;
    }
    k = 0;
    for (int l = start_row; l <= end_row; l++, k++){
        data[l] = temp[k];
        if (feild_values_name) feild_values_name[l] = temp_fv_name[k];
        else feild_values_int[l] = temp_fv[k];
        
    }
    
}

void sort(char ** data, char ** feild_values_name, double* feild_values_int, int start_row, int end_row){
    
    if (start_row >= end_row) return;
    
    int mid = (start_row + end_row)/2;
    sort(data, feild_values_name,feild_values_int, start_row, mid);
    sort(data, feild_values_name,feild_values_int, mid + 1, end_row);
    merge(data, feild_values_name, feild_values_int, start_row, end_row, mid);
}

char * grade_cal(double marks){
    if (marks >= 90) return "EX";
    else if (marks >= 80) return "A";
    else if (marks >= 70) return "B";
    else if (marks >= 60) return "C";
    else if (marks >= 50) return "D";
    else if (marks >= 40) return "P";
    
    return "F";
}
int handle_numrow() {

    FILE* fp = fopen("student_info.txt", "r");
    if (fp == NULL) return -1;
    int linecount = 0;
    char line[LINELEN];
    while(fgets(line, LINELEN, fp))
        linecount++;
    fclose(fp);
    return linecount;
}
int handle_insert(char * arg){

    
    
    if (arg == NULL) return -1;
    int cur_count = 0;
    char record[256];
    strcpy(record, arg);

    char * token = strtok(record, "\t");
    char* rollno = token;
    while(token != NULL){
        cur_count++;

        if (cur_count >= 3 && cur_count <= 7){
            if (atoi(token) <= 0) return -1;
        }
        token = strtok(NULL, "\t");
    }

    if (cur_count != 7) return -1;

    strcpy(record, arg);
    printf("recod|%s|\n", record);
    printf("recod|%s|\n", arg);


    char *rollrecord = strtok(record, "\t");
    char *  dump = strtok(record, "\n");
    FILE* fp = fopen("student_info.txt", "r");
    if (fp == NULL) return -2;

    char line[LINELEN];
    while(fgets(line, LINELEN, fp)){
        char * roll = strtok(line, "\t");
        char * rest = strtok(NULL,"\n");
       
        if (strcmp(rollrecord, roll) == 0) {
            fclose(fp);
            return -3;
        }

    }
    fclose(fp);



    FILE *fpW = fopen("student_info.txt", "a");
    if (fpW == NULL) return -2;
    fprintf(fpW, "\n%s", arg);
    fclose(fpW);
    return 1;  
}
double handle_average(int arg){
    FILE* fp = fopen("student_info.txt", "r");
    if (fp == NULL) return -1;
    double sum = 0;
    int linecount = 0;
    char line[LINELEN];
    while(fgets(line, LINELEN, fp)){
        linecount++;
        char * roll = strtok(line, "\t");
        char * name = strtok(NULL,"\t");
        char * marks = NULL;
        
        int i = arg;
        while (i--){
            marks = strtok(NULL, "\t");
        }
        
        sum += atoi(marks);

    }
    fclose(fp);
    return sum/linecount;
}


char** handle_gradex_all(int  numrow){
    char ** output = (char**) malloc(sizeof(char *)*numrow);
    for (int i = 0; i < numrow ;i++){
        output[i] = (char*) malloc(sizeof(char)*256);
    }

    FILE* fp = fopen("student_info.txt", "r");
    if (fp == NULL) return NULL;

    char line[LINELEN];
    int k = 0;
    while(fgets(line, LINELEN, fp)){
        char * roll = strtok(line, "\t");
        char * name = strtok(NULL,"\t");
        char * marks = NULL;
        int sum = 0;
        int i = 5;
        while (i--){
            marks = strtok(NULL, "\t");
            sum += atoi(marks);
        }
        

        char* grade = grade_cal(sum/5);
        if(k < numrow)
            sprintf(output[k++], "%s\t%s\t%s", roll, name, grade);
       
    }
    
    fclose(fp); 
    return output;
}

char * handle_gradex_individual(char* argroll){
    int nr = handle_numrow();
    char ** grades = handle_gradex_all(nr);
    char * output = (char*) malloc(sizeof(char) * LINELEN);
    
    int flag = 0;
    
    for(int i = 0; i < nr; i++){
        char * roll = strtok(grades[i], "\t");
        char * name = strtok(NULL, "\t");
        char * grade = strtok(NULL, "\t");
        if (atoi(argroll) == atoi(roll)){
            sprintf(output,"%s\t%s\t%s", roll, name, grade);
            flag = 1;
        }

    }
    if (flag == 0) sprintf(output,"%s","Roll number not found.");;

    return output;
}


char** handle_sortx(char * feild, int start_row, int end_row, int nr){

    if (start_row > end_row || start_row < 0 || end_row >= nr) return NULL;
    FILE* fp = fopen("student_info.txt", "r");
    if (fp == NULL) return NULL;
    char ** data = (char**) malloc(sizeof(char *)*nr);
    
    for (int i = 0; i < nr ;i++){
        data[i] = (char*) malloc(sizeof(char)*256);
    }
    char ** feild_values_name = (char**) malloc(sizeof(char *)*nr);
    for (int i = 0; i < nr ;i++){
        feild_values_name[i] = (char*) malloc(sizeof(char)*256);
    }

    double feild_values_int[nr];
    char line[LINELEN];
    int flag = 0;
    int i = 0;
    while(fgets(line, LINELEN, fp)){
        if(i < nr) 
            strcpy(data[i], line);

        char * roll = NULL, *name =NULL, *a1 =NULL, *a2 =NULL, *a3 =NULL, *a4 = NULL, *a5 = NULL;
        int cout = 0;
        char * token = strtok(line, "\t");
        while (token != NULL){
            cout++;
            if (cout == 1) roll = token;
            else if(cout == 2) name = token;
            else if (cout == 3) a1 = token;
            else if (cout == 4) a2 = token;
            else if (cout == 5) a3 = token;
            else if (cout == 6) a4 = token;
            else if (cout == 7) a5 = token;

            token = strtok(NULL, "\t");
        } 
       
        // printf("%s|%s|%s|%s|%s|%s|%s|\n", roll, name, a1, a2, a3, a4, a5);
        if (strncmp(feild, "R", 1) == 0) feild_values_int[i] = atoi(roll);
        else if (strncmp(feild, "S", 1) == 0) strcpy(feild_values_name[i], name);
        else if (strncmp(feild, "A_1", 3) == 0) feild_values_int[i] = atoi(a1);
        else if (strncmp(feild, "A_2", 3) == 0) feild_values_int[i] = atoi(a2);
        else if (strncmp(feild, "A_3", 3) == 0) feild_values_int[i] = atoi(a3);
        else if (strncmp(feild, "A_4", 3) == 0) feild_values_int[i] = atoi(a4);
        else if (strncmp(feild, "A_5", 3) == 0) feild_values_int[i] = atoi(a5);
        else if (strncmp(feild, "rank", 4) == 0) feild_values_int[i] = (atoi(a1) + atoi(a2) + atoi(a3) + atoi(a4) + atoi(a5))/5.0;
        else return NULL;
        i++;
    }
    
    if (strncmp(feild, "S", 1) != 0) feild_values_name = NULL;

    sort(data, feild_values_name, feild_values_int, start_row, end_row);

    return data;
}

char** handle_rankx_all(int nr){
    char** lines = handle_sortx("rank", 0, nr - 1, nr);
    char** output = (char**) malloc(sizeof(char*) * 10);

    for (int i = 0; i < nr; i++){
        output[i] = (char*) malloc(sizeof(char) * 256); 
    } 

    for (int i = nr - 1, j = 1; j <= 10; i--, j++){
        char* rollstr = strtok(lines[i], "\t");
        char* name = strtok(NULL, "\t");
        sprintf(output[j - 1], "%d\t%s\t%s\n", j, rollstr, name);
        
    }
    return output;
}

char* handle_rankx_individual(int nr, int roll){
    char** lines = handle_sortx("rank", 0, nr - 1, nr);
    char* output = NULL; 
    for (int i = nr - 1, j = 1; i >= 0; i--, j++){
        char* rollstr = strtok(lines[i], "\t");
        char* name = strtok(NULL, "\t");

        if (atoi(rollstr) == roll){
            output = (char*) malloc(sizeof(char) * 256);
            sprintf(output, "%d\t%s\t%s\n", j, rollstr, name);
            break;
        }
    }
    return output;
}

char** handle_similarx(int assign_i, int nr){


    char feild[4] = "A_x";
    feild[2] = '0' + assign_i;
    char** lines = handle_sortx(feild, 0, nr - 1, nr);
    char** output = (char**) malloc(sizeof(char*) * (nr + EXTRA));

    for (int i = 0; i < nr + EXTRA; i++){
        output[i] = (char*) malloc(sizeof(char) * 256); 
    } 

    record data[nr + 1];
    for (int i = 0; i < nr; i++){
        char* rollstr = strtok(lines[i], "\t");
        char* name = strtok(NULL, "\t");
        char * marks;
        int j = assign_i;
        
        while(j--) marks = strtok(NULL, "\t");
        data[i].name = name;
        data[i].roll = rollstr;
        data[i].marks = atoi(marks);
    }
    data[nr].marks = -1;
    int cur_marks = data[0].marks;
    int k = 0;
    for (int i = 0, j = 1; j < nr + 1; j++){
        if (data[j].marks != cur_marks){
            if(j - i > 1){
                for (int l = i; l < j; l++)
                    sprintf(output[k++], "%s\t%s\t%d\n", data[l].roll, data[l].name, data[l].marks);
                sprintf(output[k++], "%s\n", "-------------------------------------");
            }
            i = j;
            cur_marks = data[j].marks;
        }
        
    }
    output[k] = NULL;
    return output;
}

int main(int argc, char *argv[])
{
    int portno = 5000;
    struct sockaddr_in serv_addr;
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    int sockfd;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        error("ERROR on binding");
    }
    listen(sockfd, 5);
    struct sockaddr_in cli_addr;
    socklen_t clilen;
    clilen = sizeof(cli_addr);
    int newsockfd;
    newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
    char buffer[256];
    int n;
    while(1)
    {
        bzero(buffer,256);
        n=read(newsockfd,buffer,sizeof(buffer));
        printf("%s",buffer);
        if(strncmp(buffer,"EXIT",4)==0){
            bzero(buffer,256);
            sprintf(buffer,"%s","CONNECTION TERMINATED");
            n=write(newsockfd,buffer,sizeof(buffer));
            break;
        }
       
        if (buffer == NULL) break;
        char * cmd = strtok(buffer, " ");
        char * arg = strtok(NULL, "\n");

        printf("%s , %s\n", cmd, arg);

        if (strncmp(cmd, "NUMROW", 6) == 0){
            int nr = handle_numrow();
            bzero(buffer, 256);
            if (arg != NULL)
                sprintf(buffer, "[Input ignored] %d\n", nr);
            else
                sprintf(buffer,"%d\n",nr);
            
        }
        else if (strncmp(cmd, "INSERT", 6) == 0){
            int status = handle_insert(arg);
            bzero(buffer, 256);
            if(status == 1)
                sprintf(buffer,"%s\n","Success");
            else if (status == -2)
                sprintf(buffer, "%s\n", "[ERROR] Unable to write in the file");
            else if (status == -1)
                sprintf(buffer, "%s\n", "[ERROR] Invalid format");
            else if (status == -3)
                sprintf(buffer, "%s\n", "[ERROR] Duplicate Roll number!");
        }
        else if (strncmp(cmd, "AVERAGE", 7) == 0){
            double average = 0;
            int asgn;
            if (arg == NULL)
                asgn = -1;
            else
                asgn = atoi(arg);

            if (asgn == -1)
                sprintf(buffer,"%s\n","[ERROR] Invalid command, AVERAGE expects an integer input.");
            else if (asgn >= 1 && asgn <= 5){
                average = handle_average(atoi(arg));
                if (average == -1)
                    sprintf(buffer, "%s\n", "Unable to open file");
                else    
                    sprintf(buffer, "%.2f\n", average);
            }
            else
                sprintf(buffer,"%s\n","[ERROR] Invalid assignment number.");
            
           
        }
        else if (strncmp(cmd, "GRADEX", 6) == 0){
            char ** grades = NULL;
            char * grade = NULL;
            int numrow = handle_numrow();
            
            if (arg == NULL) 
                grades = handle_gradex_all(numrow);
            else
                grade = handle_gradex_individual(arg);

            bzero(buffer, 256);

            if (grades != NULL){
                sprintf(buffer, "%s\n", "STREAM");
                n=write(newsockfd,buffer,sizeof(buffer));
                for (int i = 0; i < numrow; i++){
                    bzero(buffer, 256);
                    sprintf(buffer, "%s\n", grades[i]);
                    n=write(newsockfd,buffer,sizeof(buffer));
                }
                sprintf(buffer, "%s\n", "DONE");
                grades = NULL;
            }
            else if(grade != NULL){
                sprintf(buffer, "%s\n", grade);
                grade = NULL;
            }
            else
                sprintf(buffer,"%s\n","[ERROR] Unable to write in the file.");
           
        }
        else if (strncmp(cmd, "SORTX", 5) == 0){
            char* feild = NULL, *start_row = NULL, *end_row = NULL;
            int start_r_i = 1, end_row_i = handle_numrow();
            if (arg != NULL){

                feild = strtok(arg, " ");
                if (feild != NULL) start_row = strtok(NULL, " ");
                if (start_row != NULL) end_row = strtok(NULL, " ");


                
                int nr = handle_numrow();

                if (start_row != NULL) start_r_i = atoi(start_row);
                if(end_row != NULL) end_row_i = atoi(end_row);

                char** lines = handle_sortx(feild, start_r_i - 1, end_row_i - 1, nr);

                if (lines == NULL){
                    sprintf(buffer, "%s\n", "Invalid argument values.");
                }
                else{
                    sprintf(buffer, "%s\n", "STREAM");
                    n=write(newsockfd,buffer,sizeof(buffer));
                    for (int i = 0; i < nr; i++){
                        bzero(buffer, 256);
                        sprintf(buffer, "%s", lines[i]);
                        n=write(newsockfd,buffer,sizeof(buffer));
                    }
                    sprintf(buffer, "%s\n", "DONE");
                    lines = NULL;
                }

            }
            else
                sprintf(buffer,"%s\n","[ERROR] Expecting arguments");
        }
        else if (strncmp(cmd, "RANKX", 5) == 0){
            char* roll = NULL, *rest = NULL; // rest is for dump
            int rolli = -1;
            int nr = handle_numrow();
            
            roll = strtok(arg, " ");
            if (roll != NULL){
                rest = strtok(NULL, "\n");
                rolli = atoi(roll);
                char* rank = handle_rankx_individual(nr, rolli);
                bzero(buffer, 256);
                if (rank == NULL)
                    sprintf(buffer, "%s\n", "Roll number not found.");
                else
                    sprintf(buffer, "%s\n", rank);
                rank = NULL;
            }
            else{
                char** ranks = handle_rankx_all(nr);
                sprintf(buffer, "%s\n", "STREAM");
                n=write(newsockfd,buffer,sizeof(buffer));
                for (int i = 0; i < 10; i++){
                    bzero(buffer, 256);
                    sprintf(buffer, "%s", ranks[i]);
                    n=write(newsockfd,buffer,sizeof(buffer));
                }
                sprintf(buffer, "%s\n", "DONE");
                ranks = NULL;
            }
            
        }
        else if (strncmp(cmd, "SIMILARX", 8) == 0){
            char* assignment = NULL;
            int assign_i;
            if (arg != NULL){

                assignment = strtok(arg, " ");
                assign_i = atoi(assignment);

                if(assign_i >= 1 && assign_i <= 5){

                    int nr = handle_numrow();
                   
                    char** lines = handle_similarx(assign_i, nr);
                    

                    sprintf(buffer, "%s\n", "STREAM");
                    n=write(newsockfd,buffer,sizeof(buffer));
                    
                    int i = 0;
                    while(lines[i]){
                        bzero(buffer, 256);
                        sprintf(buffer, "%s", lines[i]);
                        n=write(newsockfd,buffer,sizeof(buffer));
                        i++;
                    }
                    sprintf(buffer, "%s\n", "DONE");
                    lines = NULL;
                }
                else
                    sprintf(buffer, "%s\n", "[ERROR] Invalid assignment number.");

                
            }
            else
                sprintf(buffer,"%s\n","[ERROR] Expecting arguments");
            
        }
        else{
            sprintf(buffer,"%s\n","[ERROR] Invalid command.");
        }

        n=write(newsockfd,buffer,sizeof(buffer));
        bzero(buffer,256);

    }
    close(newsockfd);
    close(sockfd);
    return 0;
}
