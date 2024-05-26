#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/wait.h>

#include "../include/utils.h"
#include "../include/queue.h"

int main (int argc, char** argv) {
    if (argc != 4) {
        printf("Please input a valid number of arguments\n");
        exit(0);
    } else {
    if (strcmp(argv[3], "FIFO") != 0 && (strcmp(argv[3], "SJF") != 0)) {
        printf("Invalid escalation policy\n");
        exit(0);
    }}

    ///////////////////////////////////////////////////////////////////////////////////////
    if (mkfifo("tmp/PipeTasks", 0666) == -1) { 
        if(errno != EEXIST) {
            perror("Couldn't create file");
        }
    }
    //create the pipe that will be responsible to communicate tasks if it doesn't exist yet

    ////////////////////////////////////////////////////////////////////
    int idGenerator = 0; //variable that will fit as an Unique ID Generator for each Task that comes from a Client
    int parallelTasks = atoi(argv[2]); //number of tasks that this Server can execute simultaneously
    int tasksInExecution = 0; //variable to monitor how many tasks are being executed simultaneously
    struct Queue* taskInQueue = createQueue(20); //data struct to store Tasks that are on waitlist to be executed
    struct Queue* InExecution = createQueue(parallelTasks); //data struct to store Tasks that are being executed
    /////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////
    while (1) {  //infinte cycle that keeps the Server running
        Task tarefa; //buffer task
        int fdtarefa = open("tmp/PipeTasks", O_RDONLY); //most important pipe, responsible for handling tasks

        while (read(fdtarefa, &tarefa, sizeof(tarefa)) > 0) {

            if (getTaskStatus(&tarefa) == 0) { //if it ain't a status request

                if (getTaskisDone(&tarefa)) { //Task that ended its execution
                    removeTaskById(InExecution, getTaskId(&tarefa)); //remove the task from the Tasks in execution data structure
                    tasksInExecution--; //decrement number of tasks executing at the present time
                }
               

                if (getTaskisDone(&tarefa) == 0) { // Task that came from Client
                    idGenerator++; //increment taskID
                    int fdtarefa2 = open("tmp/PipeIDs", O_WRONLY); //open pipe to "send" the taskID to the Client
                    write(fdtarefa2, &idGenerator, sizeof(int));
                    close(fdtarefa2); //close descriptor
                    setTaskId(idGenerator, &tarefa); //"attach" the taskID to the task 
                    enqueue(taskInQueue, tarefa); //store the task in the waitlist
                }

        //if the number of tasks being executed is smaller than the fixed number of tasks that can be executed in parallel and if the waitlist ain't empty 
                if (tasksInExecution < parallelTasks && !(isEmpty(taskInQueue))) {
                    Task firstInqueue;
                    if (strcmp(argv[3], "FIFO") == 0) {
                        firstInqueue = front(taskInQueue); //get a copy of the first task of the queue
                    } else if (strcmp(argv[3], "SJF") == 0) {
                        firstInqueue = getMinInputedTimeTask(taskInQueue);
                    }
                   
                    removeTaskById(taskInQueue, getTaskId(&firstInqueue)); //remove the task from the waitlist since is gonna be executed
                    enqueue(InExecution, firstInqueue);//store the task in the executing queue
                

                    //if the flag given with the task was "-u"
                    if (strcmp(getTaskFlag(&firstInqueue), "-u") == 0) {
                        if (mkdir(argv[1], 0777) == -1) {
                            // Check if the error is because the directory already exists
                            if (errno != EEXIST) {
                                perror("Couldn't create directory");
                            }
                        }

                        //fork that is gonna separate the execution part of the server and makes sure that the server keeps being able to receive tasks
                        if (fork() == 0) {
                            char *args[20]; // Assuming a maximum of 20 arguments 
                            splitString(getTasktoExecute(&firstInqueue), " ", args); //Split the task arguments to execute
                            struct timeval start, end;
                            gettimeofday(&start, NULL); //start counting time of execution of task


                            //fork that is gonna actually execute the task
                            if (fork() ==  0) {
                                char fileoutput[20];//file name buffer
                                snprintf(fileoutput, sizeof(fileoutput), "%s/Task%d.txt", argv[1], getTaskId(&firstInqueue)); //init the output file name
                                int fdFO = open(fileoutput, O_CREAT | O_APPEND |O_WRONLY, 0666); //create the output file
                                dup2(fdFO, 1); //redirect system.out executions to Task"id".txt
                                dup2(fdFO, 2); //redirect system.err

                                char *TaskID = createTaskString(getTaskId(&firstInqueue)); //header for the output file
                                write(1, TaskID, strlen(TaskID)); //write header
                                free(TaskID); //free memory

                                execvp(args[0], args); //execute the program and its arguments + write execution output in the output file
                                perror("exec failed\n"); //if it reaches this line it means there was an error, because the exec should have killed the process
                                _exit(1);
                            } else {
                                char fileoutput[20];
                                snprintf(fileoutput, sizeof(fileoutput), "%s/Task%d.txt", argv[1], getTaskId(&firstInqueue)); //init the output file name
                                int fdFO = open(fileoutput, O_CREAT | O_APPEND |O_WRONLY, 0666);

                                wait(NULL); //wait for the child process
                                gettimeofday(&end, NULL); //stop counting the time after waiting for the child process to "die"
                                int timeExec = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec) / 1000;
                                char* time = createTaskTime(timeExec); //create baseboard for output file
                                write(fdFO, time, strlen(time)); //write baseboard for output file 
                                free(time); //free time 
                                close(fdFO); //close file descriptor
                                TaskisDone(&firstInqueue); //set done atribute to done = 1
                                int fdtarefaW = open("tmp/PipeTasks", O_WRONLY); //init pipe descriptor
                                write(fdtarefaW, &firstInqueue, sizeof(firstInqueue)); // write in "PipeTasks"
                                close(fdtarefaW); //close pipe descriptor

                                char* tasklinedone = createTaskLineDone(getTaskId(&firstInqueue), getTasktoExecute(&firstInqueue), timeExec); //create line for a done task
                                int fdTasksDone = open("output/tasksDone.txt", O_WRONLY | O_CREAT | O_APPEND, 0666); //open the file descriptor to store all the done tasks
                                write(fdTasksDone, tasklinedone, strlen(tasklinedone)); //write the line in the done tasks file
                                close(fdTasksDone); //close file descriptor
                                _exit(0);
                            }
                            _exit(0);
                        }
                    } else { ////////////  //if the flag given with the task was "-p"
                        if (mkdir(argv[1], 0777) == -1) {
                            // Check if the error is because the directory already exists
                            if (errno != EEXIST) {
                                perror("Couldn't create directory");
                            }
                        }

                        char *commands[20]; //assuming a max of 20 programs/arguments
                        int occupancy = fillArray(commands, getTasktoExecute(&firstInqueue)); //separating the programs but not its arguments if they exist 
                        if (fork() == 0) {                                                    //and find out the number of programs we're gonna execute together
                            
                            int fdIN = 0; //pipe descriptor thats gonna connect the stdin of a program with the stdout of the last program
                            int timeExec; //variable to store the time of the task
                            char* time; //buffer for the baseboard
                            struct timeval start, end;
                            gettimeofday(&start, NULL); //start counting time of execution of task
                            for(int i=0; i<occupancy; i++) { //1 iteration per program
                                int fd[2]; //nameless pipe to connect the stdouts and stdins of prgrams
                                pipe(fd);
                                if (fork() ==  0) {
/* if it ain't the first program*/  if (i!=0) dup2(fdIN, 0); //define stdin for this process
 /* if it ain't the last program*/  if (i < occupancy - 1) {
                                        dup2(fd[1], 1); //define stdout for this process
                                    }
                                    close(fd[0]); // Closes the reading side of the pipe 
                                    if (i == occupancy-1) { //if it is the last program it will write the output in the output file
                                        char fileoutput[20];
                                        snprintf(fileoutput, sizeof(fileoutput), "%s/Task%d.txt", argv[1], getTaskId(&firstInqueue));
                                        int fdFO = open(fileoutput, O_CREAT | O_APPEND |O_WRONLY, 0666);
                                        dup2(fdFO, 1); //redirect system.out executions to Task"id".txt
                                        dup2(fdFO, 2); //redirect system.err

                                        char *TaskID = createTaskString(getTaskId(&firstInqueue));
                                        write(1, TaskID, strlen(TaskID));
                                        free(TaskID);
                                    }
                                    exec_command(commands[i]); //execute and parse a program if there exist other arguments + writes in the pipe
                                    perror("exec failed\n");
                                    _exit(1);
                                } else {
                                    wait(NULL); //wait for child process
                                    close(fd[1]); //close the writing side of the pipe
                                    fdIN = fd[0]; //next program will receive the output for his input
                                    if (i == occupancy - 1) {
                                        gettimeofday(&end, NULL); //finish counting
                                        timeExec = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec) / 1000;
                                        time = createTaskTime(timeExec);
                                        char fileoutput[20];
                                        snprintf(fileoutput, sizeof(fileoutput), "%s/Task%d.txt", argv[1], getTaskId(&firstInqueue));
                                        int fdFO = open(fileoutput, O_CREAT | O_APPEND |O_WRONLY, 0666);

                                        write(fdFO, time, strlen(time));
                                        free(time);
                                        close(fdFO);
                                        TaskisDone(&firstInqueue); //set atribute done to done = 1
                                        int fdtarefaW = open("tmp/PipeTasks", O_WRONLY);
                                        write(fdtarefaW, &firstInqueue, sizeof(firstInqueue));
                                        close(fdtarefaW);

                                        char* tasklinedone = createTaskLineDone(getTaskId(&firstInqueue), getTasktoExecute(&firstInqueue), timeExec);
                                        int fdTasksDone = open("output/tasksDone.txt", O_WRONLY | O_CREAT | O_APPEND, 0666);
                                        write(fdTasksDone, tasklinedone, strlen(tasklinedone));
                                        close(fdTasksDone);
                                    }
                                }
                            }
                            _exit(0);
                        }
                        }
                        tasksInExecution++; //increment tasks in execution
                }
            } else { //is a status request
                char* inQueue = createQueueStatusInQueue(taskInQueue); //string with all the info from the tasks waitlist data structure
                char* executing = createQueueStatusExecuting(InExecution); //string with all the info from the data structure of the executing tasks

                int fdTasksDone = open("output/tasksDone.txt", O_RDONLY); //Open in read-only mode
                if (fdTasksDone == -1) {
                    perror("Error opening file");
                }

                off_t tamanho = lseek(fdTasksDone, 0, SEEK_END); //size of the file that stores all the done tasks
                lseek(fdTasksDone, 0, SEEK_SET); // Reset file pointer to the beginning
                char* statusDone = malloc(tamanho + 1 + 15);
                strcpy(statusDone, "--Done--\n\n"); // Initialize statusDone with "--Done--\n\n"
                char* linesDone = malloc(tamanho + 1); // Allocate memory

                if (linesDone == NULL) {
                    perror("Memory allocation failed");
                }

                // Read content from the file into linesDone if the file is not empty
                ssize_t bytes_read = 0;
                if (tamanho > 0) {
                    bytes_read = read(fdTasksDone, linesDone, tamanho);
                    if (bytes_read == -1) {
                        perror("Error reading file");
                        exit(EXIT_FAILURE);
                    }
                }

                // Null-terminate linesDone if bytes were read
                if (bytes_read > 0) {
                    linesDone[bytes_read] = '\0';
                }

                strcat(statusDone, linesDone); // Concatenate linesDone to statusDone
                statusDone[strlen(statusDone)] = '\0'; // Ensure statusDone is properly null-terminated
                close(fdTasksDone);

                char status[4500];
                strcpy(status, inQueue); // Initialize status with inQueue
                strcat(status, executing); // Concatenate executing to status
                strcat(status, statusDone); // Concatenate statusDone to status

            
                int fdStatus = open("tmp/PipeStatus", O_WRONLY);
                write(fdStatus, status, strlen(status)); //write status message thats gonna be read at the other side of the pipe by the client
                close(fdStatus);

                // Empty the status buffer
                strcpy(status, "");
                strcpy(statusDone, "");
                strcpy(linesDone, "");

                free(inQueue);
                free(executing);
                free(linesDone);
                free(statusDone);
            }
        }
    close(fdtarefa);
    }
    //////////////////////////////////////////////////////
}

