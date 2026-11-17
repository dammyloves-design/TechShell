#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <ctype.h>


char *outputFile = NULL;
char *inputFile = NULL;
int appendMode = 0;
char *history[100];
int historyCount = 0;

void printError(){
    fprintf(stderr, "Error %d (%s)\n", errno, strerror(errno));
}

void parseCommand(char input[], char * args[]){
    outputFile = NULL;
    inputFile = NULL;
    appendMode = 0;

    char *tokens[100];
    int protectedToken[100];
    int tokenCount = 0;

    char *read = input;
    char *write = input;

    while(*read != '\0' && tokenCount < 99){

        while(isspace((unsigned char)*read)){
            read++;
        }

        if(*read == '\0'){
            break;
        }

        tokens[tokenCount] = write;
        
        int inQuotes = 0;
        int wasProtected = 0;

        while(*read != '\0' && (inQuotes || !isspace((unsigned char)*read))){

            // Handle backslash
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

            *write = *read;
            write++;
            read++;
        }

        // Move past the space separating this token
        if(*read != '\0'){
            read++;
        }

        *write = '\0';
        write++;

        protectedToken[tokenCount] = wasProtected;
        tokenCount++;
    }

    // Step 2: Separate arguments from redirection operators
    int i = 0;
    int j = 0;

    while(i < tokenCount){

        if(!protectedToken[i] && strcmp(tokens[i], ">>") == 0){

            if(i + 1 < tokenCount){
                outputFile = tokens[++i];
                appendMode = 1;
            }
        }
        else if(!protectedToken[i] && strcmp(tokens[i], ">") == 0){

            if(i + 1 < tokenCount){
                outputFile = tokens[++i];
                appendMode = 0;
            }
        }
        else if(!protectedToken[i] && strcmp(tokens[i], "<") == 0){

            if(i + 1 < tokenCount){
                inputFile = tokens[++i];
            }
        }
        else{
            args[j] = tokens[i];
            j++;
        }
        i++;
    }
    args[j] = NULL;

}


void executeCommand(char *args[]){

    if(strcmp(args[0], "exit") == 0){
        for(int i = 0; i < historyCount; i++){
            free(history[i]);
        }
        
        exit(0);
    }

    if(strcmp(args[0], "history") == 0){
        for(int i = 0; i < historyCount; i++){
            printf("%d %s\n", i + 1, history[i]);
        }
        return;
    }

    if(strcmp(args[0], "cd") == 0){
        if(args[1] == NULL){
            if(chdir(getenv("HOME")) != 0){
                printError();
            }
        }
        else{
            if(chdir(args[1]) != 0){
                printError();
            }
        }
        return;
    }

    pid_t pid = fork();

    if(pid < 0){
        printError();
    }
    else if(pid == 0){

        if(outputFile != NULL){
            int flags = O_WRONLY | O_CREAT;

            if(appendMode == 1){
                flags = flags | O_APPEND;
            }
            else{
                flags = flags | O_TRUNC;
            }
            int fd = open(outputFile, flags, 0644);

            if(fd < 0){
                printError();
                exit(EXIT_FAILURE);
            }

            if(dup2(fd, STDOUT_FILENO) < 0){
                printError();
                exit(EXIT_FAILURE);
            }
            close(fd);
        }

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
        

        execvp(args[0], args);
        printError();
        exit(EXIT_FAILURE);
    }
    else{
        waitpid(pid, NULL, 0);
    }
}



int main(){
    
    char cwd[1024];
    char input[100];
    char *args[100];

    while (1){

        if(getcwd(cwd, sizeof(cwd)) != NULL){
            printf("%s$ ", cwd);
        }
        else{
            printError();
        }

        if(fgets(input, sizeof(input), stdin) == NULL){
            printf("\n");
            break;
        }
        input[strcspn(input, "\n")] = '\0';

        if(strlen(input) > 0 && historyCount < 100){
            history[historyCount] = strdup(input);
            historyCount++;
        }
        
        

        
        parseCommand(input, args);

        

        if(args[0] == NULL){
            continue;
        }
        
        executeCommand(args);

        
    }
    
    
    return 0;
}