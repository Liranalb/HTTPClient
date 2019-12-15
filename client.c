#include <stdio.h>
#include <string.h>
#include <stdlib.h> //check if needed
//#include <getopt.h>
#include <ctype.h>

#define DEFAULT_PORT 80

//---------Struct for storing the command arguments ---------
typedef struct{ // CHECK ABOUT ERRORS WHEN CHANGING THE LINES
    char* host; // "www...."
    int port;
    char* path;
    char* command; //"GET" or "POST"
    int pam_count;
    char* text;
}command_t;
//------------------------------------------------------------

int urlOrganizer(char *url, command_t **cmd){
    printf("\n Full temp-URL: %s\n", url);
    const char portMarker = ':';
	//add http:// checker/ check if it's the right place for it
    if(strchr(url+7, portMarker) == NULL) { //if we didn't find the marker ':', set the port to 80
		char *urlTmp = strtok(url, "/;");
		urlTmp = strtok(NULL, "/;");
		//printf("\n Host temp is: %s", urlTmp);
		(*cmd)->host = urlTmp;
		//printf("\n Host at struct is: %s", (*cmd)->host);
		urlTmp = strtok(NULL, "");
		
		//printf("\n path in temp is: %s", urlTmp);
		(*cmd)->path = urlTmp;
		//printf("\n path in struct is: %s", (*cmd)->path);
		(*cmd)->port = 80;
		//printf("\n port in struct is: %d", (*cmd)->port);
		
    }

    else { //the is a port
		char *urlTmp = strtok(url, "/;");
		urlTmp = strtok(NULL, "/;");
		
		(*cmd)->path = strtok(NULL, ":");
		//printf("\n Host temp is: %s", urlTmp);
		//printf("\n path in struct is: %s", (*cmd)->path);
		(*cmd)->host = strtok(urlTmp, ":/");
		urlTmp = strtok(NULL, ":/"); //port
		(*cmd)->port = atoi(urlTmp);
		if(((*cmd)->port) < 0) {
			printf("/nusage error no port");
			exit(1);
		}
    }

    if( (*cmd)->path == NULL) // define the root as a path in case no path given
        (*cmd)->path = (char *) '/'; //check if working

    return 0;
};


void parseCommand(int argc, char **argv){
    command_t *command = (command_t*)malloc(sizeof(command)); //will be use to store the command data and arguments
    if(!command){
        printf("cannot allocate initiate memory for command\n");
        exit(1); //check the exit code
    }

    int i;
    for(i = 1 ; i < argc ; i++){ //going over argv
        //       printf("\n #arv[%d] is: %s\n", i,  argv[i]);
        if((strncmp("http://", argv[i], 7) == 0)) { //url has been found
            //printf("\n #http is: %s\n",  argv[i]);
            //printf("\n #cut http: %s\n",  argv[i]+7);
            if(urlOrganizer(argv[i], &(command)) == -1)
                exit(1);
        }

        else { //CHECK THIS CONDITION = URL NOT FOUND
            if(i >= argc) { //if reached the end of argv
                perror("Command doesn't contains any host");
                free(command);
                exit(1);
            }
        }

        if((strncmp("-p", argv[i], 7) == 0)) {
            printf("inside http://");
        }

        if((strncmp("ir", argv[i], 7) == 0)) {
            printf("inside http://");
        }


    }
	printf("********************************\n");
    printf("*** Struct Checker From Main ***\n");
    printf("********************************\n");
    printf("The host is: %s\n", command->host);
    printf("The Path is: %s\n", command->path);
    printf("The port is: %d", command->port);
    
    free(command);
}


int main(int argc,char *argv[]) {
/*-------------------print argc and argv-----------------
    int i;

    for(i = 1 ; i < argc ; i++ ){
        printf("%s\n" ,argv[i]);
    }
*/
    parseCommand(argc, argv);
    return 0;
}
