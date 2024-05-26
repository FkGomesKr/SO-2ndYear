#include <stdio.h>
#include <string.h>

#include "../include/task.h"

void setTaskTime(int time, Task* task) {
    task->inputed_time_ms = time;
}

void setTaskFlag(char* flag, Task* task) {
    strcpy(task->flag, flag);
}

void setTasktoExecute(char* toExecute, Task* task) {
    strcpy(task->toExecute,toExecute);
}

void setTaskStatus(int status, Task* task) {
    task->status = status;
}

void setTaskId(int id, Task* task) {
    task->id = id;
}

void setTaskDone(Task* task) {
    task->done = 0; 
}

void TaskisDone(Task* task) {
    task->done = 1; 
}

int getTaskTime(Task* task) {
    return task->inputed_time_ms;
}

char* getTaskFlag(Task* task) {
    return strdup(task->flag);
}

char* getTasktoExecute(Task* task) {
    return strdup(task->toExecute);
}

int getTaskStatus(Task* task) {
    return task->status;
}

int getTaskId(Task* task) {
    return task->id;
}

int getTaskisDone(Task* task) {
    return task->done;
}

