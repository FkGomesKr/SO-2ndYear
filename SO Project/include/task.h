#ifndef TASK_H
#define TASK_H

typedef struct {
    int inputed_time_ms;
    char flag[3];
    char toExecute[278];
    int status;
    int id;
    int done;
} Task;

void setTasktoExecute(char* toExecute, Task* task);
void setTaskTime(int time, Task* task);
void setTaskFlag(char* flag, Task* task);
void setTaskStatus(int status, Task* task);
void setTaskId(int id, Task* task);
void setTaskDone(Task* task);

void TaskisDone(Task* task);

int getTaskTime(Task* task);
char* getTaskFlag(Task* task);
char* getTasktoExecute(Task* task);
int getTaskStatus(Task* task);
int getTaskId(Task* task);
int getTaskisDone(Task* task);

#endif