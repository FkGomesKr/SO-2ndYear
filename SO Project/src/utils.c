#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "../include/utils.h"

void splitString(char* str, char* delimiter, char *args[]) {
    int arg_count = 0;
    char *arg = strtok((char *)str, delimiter);

    // Loop through the string and extract arguments
    while (arg != NULL) {
        args[arg_count++] = arg;
        arg = strtok(NULL, delimiter);
    }
    args[arg_count] = NULL;
}

char* createTaskString(int taskId) {
    // Calculate the length of the formatted string
    int length = snprintf(NULL, 0, "---------------- Task ID: %d\n", taskId);
    
    // Allocate memory for the formatted string
    char* result = (char*)malloc(length + 1); // +1 for the null terminator
    
    if (result == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    
    // Create the formatted string
    snprintf(result, length + 1, "---------------- Task ID: %d\n", taskId);
    
    return result;
}

char* createTaskLineDone(int taskId, char* toExecute, int time) {
    // Calculate the length of the formatted string
    int length = snprintf(NULL, 0, "%d %s %dms\n", taskId, toExecute, time);
    
    // Allocate memory for the formatted string
    char* result = (char*)malloc(length + 1); // +1 for the null terminator
    
    if (result == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    
    // Create the formatted string
    snprintf(result, length + 1, "%d %s %dms\n", taskId, toExecute, time);
    
    return result;
}


char* createTaskTime(int tasktimeMS) {
    // Calculate the length of the formatted string
    int length = snprintf(NULL, 0, "---------------- Task took %dms\n\n", tasktimeMS);
    
    // Allocate memory for the formatted string
    char* result = (char*)malloc(length + 1); // +1 for the null terminator
    
    if (result == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    
    // Create the formatted string
    snprintf(result, length + 1, "---------------- Task took %dms\n\n", tasktimeMS);
    
    return result;
}

int fillArray(char *array[], char *command) {
    const char *delim = "|";
    char *token;
    int index = 0;
    int occupancy = 0;

    // Get the first token
    token = strtok(command, delim);

    // Walk through other tokens
    while (token != NULL && index < 3) {
        // Remove leading and trailing whitespace
        while (*token == ' ')
            token++;
        int len = strlen(token);
        while (len > 0 && token[len - 1] == ' ')
            token[--len] = '\0';

        // Allocate memory for the token and copy it to the array
        array[index] = (char *)malloc((strlen(token) + 1) * sizeof(char));
        strcpy(array[index], token);

        // Get the next token
        token = strtok(NULL, delim);
        index++;

        occupancy++;
    }

    // Fill remaining slots with NULL
    while (index < 3) {
        array[index] = NULL;
        index++;
    }

    return occupancy;
}

int exec_command(char* arg) {
    char *exec_args[10];
    char *string;
    int i = 0;

    char* command = strdup(arg);
    string = strtok(command, " ");
    
    while (string != NULL) {
        exec_args[i] = string;
        string = strtok(NULL, " ");
        i++;
    }
    exec_args[i] = NULL;
    
    return execvp(exec_args[0], exec_args);
}