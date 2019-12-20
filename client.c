#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include <ctype.h>

#define DEFAULT_PORT 80
#define SPACE ' '
#define EQUAL '='
#define USAGE_ERR "Usage: client [-p <text>] [-r n < pr1=value1 pr2=value2 ...>] <URL>"

//---------Struct for storing the command arguments ---------
typedef struct{ // CHECK ABOUT ERRORS WHEN CHANGING THE LINES
    int port;
    int post;
    int get;
    int arg_count;
    char* host; // "www...."
    char* argu;
    char* path;
    char* text;
    
}command_t;
//------------------------------------------------------------

//int request(command_t **cmd, char **argv, int argc, int *i); //EDIT

int request(command_t **cmd, char **argv, int argc, int *i){ //in case there is -r
	*i = *i + 2; // add a check for the string length
	int stringC = *i; // for the secound loop
	int count = 0;
	int argCount = (*cmd)->arg_count;
	int equalFlag = 0; 
	//printf("\n# Start REQUEST. i = %d\n", *i);
	//printf("# argCount+(*i) = %d\n", (argCount+(*i)));
	int argcT = argCount+(*i);
	int lengthCounter = (*cmd)->arg_count; //will save the length of all of the argument

	
	
	for(; (*i) < argcT; (*i)++){ // this loop continue the loop from parseCommand method.	
		char* current = argv[(*i)];

		if(strlen(current) < 3)// check if the string has a minimum of 3 chars "a=v"
			return -1;
		
		
		if(current[0] == '=' || current[strlen(current)-1] == '=') //checking if the equal sign is in the wrong place
			return -1; //the = is on the first or last argument. usage error
	
		if(strchr(current,SPACE) != NULL)
			return -1;
		
		
		int j = 1;
		while(j<(strlen(current))-1){ //the string is longer then 3, with no space and no "=" on the first and last char
			char tmp = current[j];
			if(tmp == '='){ //check and count the spaces in the middle
				equalFlag++;
				if(equalFlag > 1) {//the is to many spaces
					return -1;
					
				}
			}	

			j++;
			//printf("\n test");
		}
		
		lengthCounter+=strlen(current);
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
	
	printf("\nThe temp text is %s\n", tmpText);
	
	((*cmd)->argu) = (char*)malloc(sizeof(char)*strlen(tmpText));
	    if(!((*cmd)->argu)) {
        printf("Cannot allocate initial memory for data\n");
        return -1;
    }
	
	strcpy(((*cmd)->argu), tmpText);
	
	printf("\nThe struct text is %s\n", (*cmd)->argu);

	if(count == argCount){
		(*i)=(*i)-1;
		return (*i);
		}
	
	else{
		free((*cmd)->argu);
		return -1;
	}	
	

}


int urlOrganizer(char *url, command_t **cmd){
    printf("\n Full temp-URL: %s\n", url);
    const char portMarker = ':';
    if(strchr(url+7, portMarker) == NULL) { //if we didn't find the marker ':', set the port to 80
        char *urlTmp = strtok(url, "/");
        urlTmp = strtok(NULL, "/");
        (*cmd)->host = urlTmp;
        urlTmp = strtok(NULL, "");
        (*cmd)->path = urlTmp;
        (*cmd)->port = 80;


    }

    else { //the is a port
        char *urlTmp = strtok(url, "/");
        urlTmp = strtok(NULL, "/");

        (*cmd)->path = strtok(NULL, ":");
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
            command->text = (char*)malloc(sizeof(char)*strlen(argv[i+1])); //save the text for post later
			if(!command->text) {
				printf("Cannot allocate initial memory for data\n");
				free(command);
				exit(1);
			}
			
            strcpy(command->text, argv[i+1]);
            //printf("\n The p text is %s", command->text);
            i++; //skip the text for the next checks
            continue;
        }
        
        if((strncmp("http://", argv[i], 7) == 0)) { //if the first seven chars of the current string is http://
            if(urlOrganizer(argv[i], &(command)) == -1){ //sent the address for proccesing
                printf(USAGE_ERR);
                free(command);
                exit(1);
			}
			continue; //done cuting the url
			
        }
		
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
						
			command->arg_count = atoi(argv[i+1]); //save the atoi counter
			
			if((request((&command), argv, argc, &i)) == -1){
                printf(USAGE_ERR);
                free(command);
                exit(1);
            }
            command->get = 1;
\
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
    printf("The port is: %d\n", command->port);
    printf("The arg's are: %s\n", command->argu);
    printf("The post text is %s", command->text);
   
	//free(command->text);
	free(command->text);
	free(command->argu);
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
