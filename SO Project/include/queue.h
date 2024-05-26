#ifndef QUEUE_H
#define QUEUE_H

#include "task.h"

struct Queue {
    int front, rear, size;
    unsigned capacity;
    Task *array;
};

struct Queue* createQueue(unsigned capacity);
void enqueue(struct Queue* queue, Task task);
void removeFront(struct Queue* queue);
Task front(struct Queue* queue);
int isEmpty(struct Queue* queue);
void removeTaskById(struct Queue* queue, int taskId);
int queueLength(struct Queue* queue);
char* createQueueStatusExecuting(struct Queue* queue);
char* createQueueStatusInQueue(struct Queue* queue);
Task getMinInputedTimeTask(struct Queue* queue);

#endif
