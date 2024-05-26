#ifndef UTILS_H
#define UTILS_H

void splitString(char* str, char* delimiter, char *args[]);
char* createTaskString(int taskId);
char* createTaskTime(int tasktimeMS);
char* createTaskLineDone(int taskId, char* toExecute, int time);
int fillArray(char *array[], char *command);
int exec_command(char* arg);

#endif