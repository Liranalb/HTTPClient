#include <stdio.h>
//#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
//#include <ctype.h>
#include <netdb.h>

#define DEFAULT_PORT 80
#define SPACE ' '
#define EQUAL '='
#define USAGE_ERR "Usage: client [-p <text>] [-r n < pr1=value1 pr2=value2 ...>] <URL>"
#define PROTOCOL "HTTP/1.0"
#define BUFLEN 10

//---------Struct for storing the command arguments and attributes ---------
typedef struct comm{ //
    int port;
    int post;
    int arg_count;
    char* host; // "www...."
    char* text;
    char* arguStr;
    char* path;
    char* strRequest;

}command_t;
//------------------------------------------------------------


void reqBuilder(command_t **cmd);
void usageERR(command_t **cmd);
void freeAlloc(command_t *cmd);

/* in case there is -r, this method will check if the arguments are valid
 * the method will return the current i after checking all the arguments, or
 * -1 if it's fails. This method also creates a string contains the arguments in
 * the right format inside the command struct.*/

int request(command_t **cmd, char **argv, int argc, int *i){
    *i = *i + 2; // add a check for the string length
    int stringC = *i; // for the second loop
    int count = 0;
    int argCount = (*cmd)->arg_count;
    int equalFlag = 0; //will be use to verify if the  argument contains only one '=' sign
    int argcT = argCount+(*i);
    int lengthCounter = (*cmd)->arg_count; //will save the length of all of the argument

    for(; (*i) < argcT; (*i)++){ // this loop continue the loop from cmdCutter method.
        char* current = argv[(*i)];

        if(strlen(current) < 3){// check if the string has a minimum of 3 chars "a=v"
            return -1;
        }

        if(current[0] == EQUAL || current[strlen(current)-1] == EQUAL){ //checking if the equal sign is in the wrong place
            return -1; //the = is on the first or last argument. usage error
        }

        if(strchr(current,SPACE) != NULL){
            return -1;
        }


        int j = 1;
        while(j<(strlen(current))-1){ //the string is longer then 3, with no space and no "=" on the first and last char
            char tmp = current[j];
            if(tmp == EQUAL){ //check and count the spaces in the middle
                equalFlag++;
                if(equalFlag > 1) {//the is to many spaces
                    return -1;
                }
            }

            j++;
            //printf("\n test");
        }

        lengthCounter+=(int)strlen(current);
        if(equalFlag == 0){
            return -1;
        }

        equalFlag = 0;
        count++;
    }

    char tmpText[lengthCounter];
    tmpText[0] = '?';

    for(; stringC < argcT; stringC++){ //after we checked the argument, entering them to the struct
        strcat(tmpText, argv[stringC]);
        if(stringC != argcT-1)
            strcat(tmpText, "&");
    }

    ((*cmd)->arguStr) = (char*)malloc(sizeof(char) * (strlen(tmpText)+1));
    if(!((*cmd)->arguStr)) {
        printf("Cannot allocate initial memory for data\n");
        return -1;
    }

    strcpy(((*cmd)->arguStr), tmpText);

    if(count == argCount){ //return the index of the first cell that comes after the arguments
        (*i)=(*i)-1;
        return (*i);
    }

    else{
        return -1;
    }
}

/* cut the url the put the components inside the struct */
int urlOrganizer(char *url, command_t **cmd){
    const char portMarker = ':';
    if(strchr(url+7, portMarker) == NULL) { //if we didn't find the marker ':', set the port to 80
        char *urlTmp = strtok(url, "/");
        urlTmp = strtok(NULL, "/");
        (*cmd)->host = urlTmp;
        urlTmp = strtok(NULL, "");
        //printf("\nurlTmp: %s\n", urlTmp);
        if(urlTmp == NULL) { // define the root as a path in case no path given
            //check if working
            ((*cmd)->path) = (char*)malloc(sizeof(char)*2);
            if(!(*cmd)->path)
                return -1;
            strcpy(((*cmd)->path), "/");
        }
        else {
            strcat(urlTmp, "\0");
            ((*cmd)->path) = (char*)malloc(sizeof(char)*strlen(urlTmp)+1);
            if(!(*cmd)->path)
                return -1;
            strcat((*cmd)->path, "/");
            strcat((*cmd)->path, urlTmp);

            //printf("\nThe path is:%s\n", t);


        }

        (*cmd)->port = DEFAULT_PORT;
    }

    else { //there is a port
        char *urlTmp = strtok(url, "/");
        urlTmp = strtok(NULL, "/");
        (*cmd)->path = strtok(NULL, ":");
        (*cmd)->host = strtok(urlTmp, ":/");
        urlTmp = strtok(NULL, ":/"); //port
        int tmpPort =  atoi(urlTmp);
        if(tmpPort <= 0) // check
            return -1;
        (*cmd)->port = tmpPort;
    }
    //printf("The Path is: %s\n", (*cmd)->path);
    //if((*cmd)->path == NULL) // define the root as a path in case no path given
      //  (*cmd)->path = "/"; //check if working
    return 0;
}


void reqBuilder(command_t **cmd) {
    char tmp[256];
    if((*cmd)->post <= 0){
        strcpy(tmp, "GET ");

    }
    else
        strcpy(tmp, "POST ");

    if(((*cmd)->path)){
        strcat(tmp, (*cmd)->path);
    }

    if((*cmd)->arguStr){
        strcat(tmp, (*cmd)->arguStr);
    }

    strcat(tmp, " ");
    strcat(tmp, PROTOCOL);
    strcat(tmp, "\r\nHOST: ");
    strcat(tmp, (*cmd)->host);
    strcat(tmp, "\r\n");
    if((!(*cmd)->post) || (*cmd)->post == 0) {
        strcat(tmp, "\r\n");
        (*cmd)->strRequest = (char*)malloc(sizeof(char)*(strlen(tmp)+1));
        if(!((*cmd)->strRequest)) {
            printf("Cannot allocate initial memory for data\n");
            freeAlloc(*cmd);
            exit(1);
        }
        strcpy((*cmd)->strRequest, tmp);
    }


    if((*cmd)->post > 0){
        strcat(tmp, "Content-length:");

        char buffer[10];
        int value = (int)strlen((*cmd)->text);
        sprintf(buffer, "%d", value);
        strcat(tmp, buffer);
        strcat(tmp, "\r\n\r\n");
        strcat(tmp, (*cmd)->text);
        strcat(tmp, "\0");
        (*cmd)->strRequest = (char*)malloc(sizeof(char)*(strlen(tmp)+1));
        if(!((*cmd)->strRequest)) {
            printf("Cannot allocate initial memory for data\n");
            freeAlloc(*cmd);
            exit(1);
        }
        strcpy((*cmd)->strRequest, tmp);
    }
}

void usageERR(command_t **cmd){ //printing the usage error and freeing up memory
    printf(USAGE_ERR);
    freeAlloc(*cmd);
    exit(1);
}


void freeAlloc(command_t *cmd){ //checking allocations and freeing them if needed
	if(cmd->strRequest)
		free(cmd->strRequest);
    if(cmd->text)
		free(cmd->text);
    if(cmd->arguStr)
		free(cmd->arguStr);
    if(cmd->path)
        free(cmd->path);
    free(cmd);
}



int main(int argc,char *argv[]) {

    command_t *command = (command_t*)malloc(sizeof(command_t)); //will be use to store the command data and arguments
    if(!command){
        printf("cannot allocate initiate memory for command\n");
        freeAlloc(command); //check
        exit(1); //check the exit code
    }

    int i;
    for(i = 1 ; i < argc ; i++){ //going over argv
        if((strcmp("-p", argv[i]) == 0)) {
            if(i == argc-1){ //-p is last on the command. not text after it
                usageERR(&command);
                exit(1);
            }

            command->post = 1; //turn on post flag
            command->text = (char*)malloc(sizeof(char)*strlen(argv[i+1]+1)); //save the text for post later
            if(!command->text) {
                printf("Cannot allocate initial memory for data\n");
                freeAlloc(command);
                exit(1);
            }

            strcpy(command->text, argv[i+1]);
            i++; //skip the text for the next checks
            continue;
        }

        if((strncmp("http://", argv[i], 7) == 0)) { //if the first seven chars of the current string is http://
            if(urlOrganizer(argv[i], &(command)) == -1){ //sent the address for proccesing
                usageERR(&command);
                exit(1);
            }
            continue; //done cuting the url

        }

        if((strcmp("-r", argv[i]) == 0)) {
            if(i == argc-1){ //check if -r is the last in the string
                usageERR(&command);
                exit(1);
            }

            if((strcmp("0", argv[i+1]) == 0)){ //check if there is no arguments
                //command->post = 0;
                i++;
                continue;
            }

            if(atoi(argv[i+1]) < 1){ //check if atoi failed
                usageERR(&command);
                exit(1);
            }

            command->arg_count = atoi(argv[i+1]); //save the atoi counter

            if((request((&command), argv, argc, &i)) == -1){
                usageERR(&command);
                exit(1);
            }
            continue;


        }
        //the current argv[i] doesn't contains "-p" "-r" "http://" or arguments. print usage error and exit
        usageERR(&command);
        exit(1);

    }

    reqBuilder(&command);
    printf("HTTP request=\n%s\nLEN=%d\n", command->strRequest, strlen(command->strRequest));
    int sockfd; //return file descriptor when succeed;

    int rc;

    struct sockaddr_in serv_addr;
    struct  hostent *server;
    char rbuf[BUFLEN];
    char* buffer = command->strRequest;

    if(argc < 3){
        printf(USAGE_ERR);
        exit(0);
    }

//---------------------------- connection--------------------------------------------------

    sockfd = socket( AF_INET, SOCK_STREAM , 0); //create an endpoint for communication
    if(sockfd < 0){ //checking id succeed
        perror("socket");
        exit(0);
    }

    server = gethostbyname(command->host);
    //printf("\nserver after is:#%s#\n" , server->h_name);
    if(!server){
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }


    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *) &serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(command->port);

    //initiate connection on a socket
    if(connect(sockfd, (const struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) { //initiate connection on a socket
        perror("ERROR connecting");
        exit(1);
    }

    rc = write(sockfd, buffer, strlen(buffer)+1);
    if(rc < 0){
        perror("write");
        exit(1);
    }

    while(1){
        if(rc == 0)
            break;
        rc = read(sockfd, rbuf, sizeof(rbuf));

        rbuf[rc] = '\0';
        printf("%s", rbuf);
    }

    close(sockfd);
    if(command)
        freeAlloc(command);


    return 0;
}
