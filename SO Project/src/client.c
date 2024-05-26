#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <strings.h>
#include <stdlib.h>
#include <string.h>

#include "../include/task.h"

int main (int argc, char** argv) {
    if (argc<2) printf("Please, input a valid number of arguments.");
    else {
        if (strcasecmp(argv[1], "Status") == 0) {
            //request the server for the tasks status

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////
            if (mkfifo("tmp/PipeStatus", 0666) == -1) {
                if(errno != EEXIST) {
                    perror("Couldn't create file");
                }
            }
            //create the pipe that will be responsible to communicate Status requests and replys if it doesn't exist yet
            
            Task tarefa; //load task
            setTaskStatus(1, &tarefa); //declare that this "task" is a status request
            int fdTask = open("tmp/PipeTasks", O_WRONLY); //open pipe to write task
            write(fdTask, &tarefa, sizeof(tarefa)); //write task to fifo pipe
            close(fdTask); //close pipe descriptor
           
            int fdStatus = open("tmp/PipeStatus", O_RDONLY); //open pipe to read the Status reply
            char status[4500];
            read(fdStatus, status, sizeof(status)); //read status messagem from pipe
            close(fdStatus); //close pipe descriptor
            printf("%s", status);

            // Unlink (destroy) the FIFO pipe
            if (unlink("tmp/PipeStatus") == -1) {
                perror("Error unlinking FIFO pipe");
            }

        }
        else if (strcasecmp(argv[1], "Execute") == 0) { //request to server to execute a task 

            if (argc < 5) {
                printf("Please, input a valid number of arguments to request the execution of a task");
            } else {
                ////////////////////////////////////////////////////////////////////////////////////////////
                if (mkfifo("tmp/PipeIDs", 0666) == -1) {
                    if(errno != EEXIST) {
                        perror("Couldn't create file");
                    }
                }
                //create the pipe that will be responsible to communicate Tasks' IDs if it doesn't exist yet

                Task tarefa; //load a task
                setTaskFlag(argv[3], &tarefa); 
                setTaskTime(atoi(argv[2]), &tarefa);
                setTasktoExecute(argv[4], &tarefa);
                setTaskStatus(0, &tarefa); //declare that this task isn't a status request
                setTaskDone(&tarefa); //by default, this will set the "done" to 0, meaning this is a task that came from the client

                int fd = open("tmp/PipeTasks", O_WRONLY); //open pipe to write task
                write(fd, &tarefa, sizeof(tarefa)); //write task to fifo pipe
                close(fd); //close pipe descriptor
    
                int id;
                int fd2 = open("tmp/PipeIDs", O_RDWR); //open pipe to read the task id
                read(fd2, &id, sizeof(int)); //stores the task id into the variable "id"
                close(fd2); //close pipe descriptor
                printf("O ID da sua tarefa é %d\n", id);


                // Unlink (destroy) the FIFO pipe
                if (unlink("tmp/PipeIDs") == -1) {
                    perror("Error unlinking FIFO pipe");
                }
            }
        }
        else printf("Please, input a valid request.");
    }
}
