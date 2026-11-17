// Name: Ayomide Oyekunle

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <ctype.h>

// Global variables used to keep track of input and output redirection
char *outputFile = NULL;
char *inputFile = NULL;
int appendMode = 0;

// Array to store command history
char *history[100];
int historyCount = 0;

// Displays error
void printError(){
    fprintf(stderr, "Error %d (%s)\n", errno, strerror(errno));
}

// Parses user input into command arguments 
void parseCommand(char input[], char * args[]){
    outputFile = NULL;
    inputFile = NULL;
    appendMode = 0;

    char *tokens[100];      // stores individual tokens frm the command
    int protectedToken[100]; // tracks tokens containng quotes or escaped characters
    int tokenCount = 0;

    // Read and write pointers allows the input string to be modified in place
    char *read = input;
    char *write = input;

    // Separate the inpuy intontokens while there is space available
    while(*read != '\0' && tokenCount < 99){

        // skip whitespace between tokens
        while(isspace((unsigned char)*read)){
            read++;
        }

        if(*read == '\0'){
            break;
        }

        tokens[tokenCount] = write;
        
        int inQuotes = 0; // tracks whether the parser is inside double quotes
        int wasProtected = 0; // tracks whehter this token contained quoted or escapes

        // continue reading until the end of the token
        while(*read != '\0' && (inQuotes || !isspace((unsigned char)*read))){

            // Handle backslash escapes, allows spaces to be part of a token
            if(*read == '\\' && read[1] != '\0'){
                wasProtected = 1;
                read++;
                *write = *read;
                write++;
                read++;
                continue;
            }

            // Handle double quotes
            if(*read == '"'){
                wasProtected = 1;
                inQuotes = !inQuotes;
                read++;
                continue;
            }

            // Copy the current character into the token
            *write = *read;
            write++;
            read++;
        }

        // Move past the space separating this token
        if(*read != '\0'){
            read++;
        }

        // End the current token with a null generator
        *write = '\0';
        write++;

        protectedToken[tokenCount] = wasProtected;
        tokenCount++;
    }

    // Separate arguments from redirection operators
    int i = 0;
    int j = 0;

    while(i < tokenCount){

        // Check for append redirection
        if(!protectedToken[i] && strcmp(tokens[i], ">>") == 0){

            if(i + 1 < tokenCount){
                outputFile = tokens[++i];
                appendMode = 1;
            }
        }
        // Check for output redirection
        else if(!protectedToken[i] && strcmp(tokens[i], ">") == 0){

            if(i + 1 < tokenCount){
                outputFile = tokens[++i];
                appendMode = 0;
            }
        }
        // Check for input redirection
        else if(!protectedToken[i] && strcmp(tokens[i], "<") == 0){

            if(i + 1 < tokenCount){
                inputFile = tokens[++i];
            }
        }
        // store tokens that are not redirection operators as command arguments
        else{
            args[j] = tokens[i];
            j++;
        }
        i++;
    }
    args[j] = NULL;

}

// Executes built-in commands
void executeCommand(char *args[]){

    // handle the exit command 
    // free allocated history memory and closing the shell
    if(strcmp(args[0], "exit") == 0){
        for(int i = 0; i < historyCount; i++){
            free(history[i]);
        }
        
        exit(0);
    }


    // handle history command - displays previous commands
    if(strcmp(args[0], "history") == 0){
        for(int i = 0; i < historyCount; i++){
            printf("%d %s\n", i + 1, history[i]);
        }
        return;
    }

    // handles changing directory "cd"
    if(strcmp(args[0], "cd") == 0){
        if(args[1] == NULL){
            if(chdir(getenv("HOME")) != 0){ // if no directory is provided change to home
                printError();
            }
        }
        else{
            if(chdir(args[1]) != 0){ // change to directory provided by user
                printError();
            }
        }
        return;
    }


    pid_t pid = fork(); // create a child process to execute external commands

    if(pid < 0){
        printError(); // display error if child process could not be created
    }
    else if(pid == 0){ // child process

        // handle output redirection if required
        if(outputFile != NULL){
            int flags = O_WRONLY | O_CREAT;

            // choose whether to appned or overwrite the file
            if(appendMode == 1){
                flags = flags | O_APPEND;
            }
            else{
                flags = flags | O_TRUNC;
            }
            int fd = open(outputFile, flags, 0644); // open or create the output file

            if(fd < 0){
                printError();
                exit(EXIT_FAILURE);
            }

            // redirect stdout to the opened file
            if(dup2(fd, STDOUT_FILENO) < 0){
                printError();
                exit(EXIT_FAILURE);
            }
            close(fd);
        }

        // handle input redirection if requested
        if (inputFile != NULL){
            int fd = open(inputFile, O_RDONLY);

            if (fd < 0){
                printError();
                exit(EXIT_FAILURE);
            }

           if(dup2(fd, STDIN_FILENO) < 0){
                printError();
                exit(EXIT_FAILURE);
            }
            close(fd);
            
        }
        
        // replace child
        execvp(args[0], args);

        // if execvp fails
        printError();
        exit(EXIT_FAILURE);
    }
    else{
        waitpid(pid, NULL, 0); // parent waits for child to finish
    }
}



int main(){
    
    char cwd[1024]; // stores current working directory
    char input[100]; // stores user'd command line
    char *args[100]; // soted parsed arguments

    while (1){

        // Display current working directory in the ahell prompt
        if(getcwd(cwd, sizeof(cwd)) != NULL){
            printf("%s$ ", cwd);
        }
        else{
            printError();
        }

        // read command from user
        if(fgets(input, sizeof(input), stdin) == NULL){
            printf("\n");
            break;
        }
        input[strcspn(input, "\n")] = '\0'; // removes newline char

        // saves a copy of each command in the history array
        if(strlen(input) > 0 && historyCount < 100){
            history[historyCount] = strdup(input);
            historyCount++;
        }
        
        

        
        parseCommand(input, args); // parse commands into arguments

        // ignore blank or whitespace-only commands
        if(args[0] == NULL){
            continue;
        }
        
        executeCommand(args); // execute parsed commad

        
    }
    
    
    return 0;
}