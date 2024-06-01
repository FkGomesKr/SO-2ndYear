#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/task.h"
#include "../include/queue.h"

// Function to create a queue of given capacity
struct Queue* createQueue(unsigned capacity) {
    struct Queue* queue = (struct Queue*)malloc(sizeof(struct Queue));
    queue->capacity = capacity;
    queue->front = queue->size = 0;
    queue->rear = capacity - 1;  // This is important, see the enqueue function
    queue->array = (Task*)malloc(queue->capacity * sizeof(Task));
    return queue;
}

// Function to check if the queue is full
int isFull(struct Queue* queue) {
    return (queue->size == queue->capacity);
}

// Function to check if the queue is empty
int isEmpty(struct Queue* queue) {
    return (queue->size == 0);
}

// Function to add a task to the queue
void enqueue(struct Queue* queue, Task task) {
    if (isFull(queue))
        return;
    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->array[queue->rear] = task;
    queue->size += 1;
}

// Function to remove a task from the queue
Task dequeue(struct Queue* queue) {
    Task emptyTask;
    emptyTask.inputed_time_ms = -1; // Indicates empty task
    if (isEmpty(queue))
        return emptyTask;

    Task task = queue->array[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size -= 1;
    return task;
}

// Function to get the front task from the queue without removing it
Task front(struct Queue* queue) {
    Task emptyTask = {0};
    if (isEmpty(queue))
        return emptyTask;
    return queue->array[queue->front];
}

Task getMinInputedTimeTask(struct Queue* queue) {
    Task emptyTask = {0};
    setTaskTime(-1, &emptyTask); // Indicates no task found
    if (isEmpty(queue))
        return emptyTask;

    Task minTask = queue->array[queue->front];
    int index = queue->front;
    for (int i = 1; i < queue->size; ++i) {
        index = (index + 1) % queue->capacity;
        if (queue->array[index].inputed_time_ms < minTask.inputed_time_ms)
            minTask = queue->array[index];
    }
    return minTask;
}

// Function to remove the front element from the queue
void removeFront(struct Queue* queue) {
    if (isEmpty(queue))
        return;

    queue->front = (queue->front + 1) % queue->capacity;
    queue->size -= 1;
}


// Function to remove a task from the queue based on its ID
void removeTaskById(struct Queue* queue, int taskId) {
    if (isEmpty(queue))
        return;

    int index = queue->front;
    while (index != queue->rear) {
        if (queue->array[index].id == taskId) {
            // Shift elements to the left to overwrite the removed task
            for (int i = index; i != queue->rear; i = (i + 1) % queue->capacity)
                queue->array[i] = queue->array[(i + 1) % queue->capacity];

            // Update rear pointer and size
            queue->rear = (queue->rear - 1 + queue->capacity) % queue->capacity;
            queue->size -= 1;
            return;
        }
        index = (index + 1) % queue->capacity;
    }

    // Check if the last element matches the task ID
    if (queue->array[queue->rear].id == taskId) {
        queue->rear = (queue->rear - 1 + queue->capacity) % queue->capacity;
        queue->size -= 1;
    }
}

// Function to get the current length of the queue
int queueLength(struct Queue* queue) {
    return queue->size;
}

char* createQueueStatusExecuting(struct Queue* queue) {
    int index = queue->front;
    int count = 0;
    char* result = malloc(300*queueLength(queue)*sizeof(char)+20); // Allocate memory for the result
    if (result == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    snprintf(result, 300*queueLength(queue)*sizeof(char)+20, "--Executing--\n\n"); // Write the "Executing" header
    if (!isEmpty(queue)) {
    int offset = strlen(result); // Track the current position in the result string
        while (count < queue->size) {
            // Append information about each task to the result string
            offset += snprintf(result + offset, 1000 - offset, "%d %s\n\n", queue->array[index].id, queue->array[index].toExecute);
            index = (index + 1) % queue->capacity;
            count++;
        }
    }
    return result; // Return the constructed result string
}


char* createQueueStatusInQueue(struct Queue* queue) {
    int index = queue->front;
    int count = 0;
    char* result = malloc(300*queueLength(queue)*sizeof(char)+21); // Allocate memory for the result
    if (result == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    snprintf(result, 300*queueLength(queue)*sizeof(char)+21, "\n--In Queue--\n\n"); // Write the "In Queue" header
    if (!isEmpty(queue)) {
        int offset = strlen(result); // Track the current position in the result string
        while (count < queue->size) {
            // Append information about each task to the result string
            offset += snprintf(result + offset, 1400 - offset, "%d %s\n\n", queue->array[index].id, queue->array[index].toExecute);
            index = (index + 1) % queue->capacity;
            count++;
        }
    }
    return result; // Return the constructed result string
}
