#include <stdio.h>
#include <string.h>
#include <stdlib.h> //check if needed
//#include <getopt.h>
#include <ctype.h>

#define DEFAULT_PORT 80
#define USAGE_ERR "Usage: client [-p <text>] [-r n < pr1=value1 pr2=value2 ...>] <URL>"

//---------Struct for storing the command arguments ---------
typedef struct{ // CHECK ABOUT ERRORS WHEN CHANGING THE LINES
    int port;
    int post;
    int get;
    int arg_count;
    char* host; // "www...."
    char* path;
    char* text;
}command_t;
//------------------------------------------------------------

int request(command_t **cmd, char **argv, int argc, int *i); //EDIT

int request(command_t **cmd, char **argv, int argc, int *i){ //in case there is -r
	int count = 0;
	arguCount = (*cmd)->arg_count);
	for(; *i < (arguCount+(*i)); i++){
		
		printf("check");
		
		count++;
	}
	if(count == arguCount)
		return *i;
	return -1;
}
//int urlOrganizer(command_t **cmd){

int urlOrganizer(char *url, command_t **cmd){
    printf("\n Full temp-URL: %s\n", url);
    const char portMarker = ':';
    //add http:// checker/ check if it's the right place for it
    if(strchr(url+7, portMarker) == NULL) { //if we didn't find the marker ':', set the port to 80
        char *urlTmp = strtok(url, "/");
        urlTmp = strtok(NULL, "/");
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
        char *urlTmp = strtok(url, "/");
        urlTmp = strtok(NULL, "/");

        (*cmd)->path = strtok(NULL, ":");
        //printf("\n Host temp is: %s", urlTmp);
        //printf("\n path in struct is: %s", (*cmd)->path);
        (*cmd)->host = strtok(urlTmp, ":/");
        urlTmp = strtok(NULL, ":/"); //port
        (*cmd)->port = atoi(urlTmp);
        if(((*cmd)->port) < 0) { //the port is 0 or negative
            return -1;
        }
    }

    if((*cmd)->path == NULL) // define the root as a path in case no path given
        (*cmd)->path = (char *) '/'; //check if working

    return 0;
};


void parseCommand(int argc, char **argv){ //change it to return the struct
    command_t *command = (command_t*)malloc(sizeof(command_t)); //will be use to store the command data and arguments
    if(!command){
        printf("cannot allocate initiate memory for command\n");
        exit(1); //check the exit code
    }

    int i;
    for(i = 1 ; i < argc ; i++){ //going over argv
		if((strcmp("-p", argv[i]) == 0)) {
            if(i == argc-1){ //-p is last on the command. not text after it
				printf(USAGE_ERR);
				free(command);
				exit(1);
			}
            
            command->post = 1; //turn on post flag
            //printf("The post value is: %d", command->post);
            //printf("\n arg[i+1] is: %s\n", argv[i+1]);
            command->text = argv[i+1]; //save the text for post later
            //printf("\n struct text is: %s\n", command->text);
            i++; //skip the text for the next checks
            continue;
        }
        
        //       printf("\n #arv[%d] is: %s\n", i,  argv[i]);
        if((strncmp("http://", argv[i], 7) == 0)) { //if the first seven chars of the current string is http://
            //printf("\n #http is: %s\n",  argv[i]);
            //printf("\n #cut http: %s\n",  argv[i]+7);
            if(urlOrganizer(argv[i], &(command)) == -1){ //sent the address for proccesing
                printf(USAGE_ERR);
                free(command);
                exit(1);
			}
			continue; //done cuting the url
			
        }

		//	else { //CHECK THIS CONDITION = URL NOT FOUND
		//		if(i >= argc) { //if reached the end of argv
		//			printf(USAGE_ERR);
		//			free(command);
		//			exit(1);
		//		}
		//	}

        if((strcmp("-r", argv[i]) == 0)) { 
			if(i == argc-1){ //check if -r is the last in the string or atoi faild
				printf(USAGE_ERR);
				free(command);
				exit(1);
			}
			
			if((strcmp("0", argv[i+1]) == 0)){ //if the is no argument. do get anyway ####CHECK
				command->get = 1;
				i++;
				continue;
			}
			
            if(atoi(argv[i+1]) < 1){ //check if atoi failed
				printf(USAGE_ERR);
				free(command);
				exit(1);
			}
			
			 //the arg counter is ok
			command->arg_count = atoi(argv[i+1]); //save the atoi counter
			
			///////CHECK ARGUMENT HERE
			//request(&command , argv[i], argc, &i);
				
			//printf("\nargv[i+1] is: %d\n", command->arg_count);
			//printf("\nargv[i+1] is: %s\n", argv[i+1]);
			//printf("\n[i+1] is: %d\n", (i+1));
			//printf("\nargc %d\n", argc);
			continue;	
			
			
        }
        //the current argv[i] doesn't contains "-p" "-r" "http://" or arguments. print usage error and exit
		printf(USAGE_ERR);
		free(command);
		exit(1);

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
