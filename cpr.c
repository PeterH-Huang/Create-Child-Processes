/* ------------------------------------------------ ------------
File: cpr.c

Last name:Huang
Student number:300021511

Last name: Wang
Student number: 300028892

Description: This program contains the code for creation
 of a child process and attach a pipe to it.
	 The child will send messages through the pipe
	 which will then be sent to standard output.

Explanation of the zombie process:
A zombie process is a completed executed process but is still active in the process table because the parent has not removed it

------------------------------------------------------------- */
#include <stdio.h>
#include <stdio.h>
#include <sys/select.h>
#include <unistd.h>
#include <stdlib.h>

/* Prototype */
void createChildAndRead (int);

/* -------------------------------------------------------------
Function: main
Arguments: 
	int ac	- number of command arguments
	char **av - array of pointers to command arguments
Description:
	Extract the number of processes to be created from the
	Command line. If an error occurs, the process ends.
	Call createChildAndRead to create a child, and read
	the child's data.
-------------------------------------------------- ----------- */

int main (int ac, char **av)
{
 int processNumber; 

 if (ac == 2)
 {
 if (sscanf (av [1], "%d", &processNumber)== 1)
 {
    createChildAndRead(processNumber);
 }
    else fprintf(stderr, "Cannot translate argument\n");
 }
    else fprintf(stderr, "Invalid arguments\n");
    return (0);
}


/* ------------------------------------------------ -------------
Function: createChildAndRead
Arguments: 
	int prcNum - the process number
Description:
	Create the child, passing prcNum-1 to it. Use prcNum
	as the identifier of this process. Also, read the
	messages from the reading end of the pipe and sends it to 
	the standard output (df 1). Finish when no data can
	be read from the pipe.
-------------------------------------------------- ----------- */

void createChildAndRead(int prcNum)
{
	//When the prcNum = 1, it will not create a child, thus only a statement needs to be printed
	if (prcNum == 1){
		printf("Process 1 begins\n");
		sleep(5);
		printf("Process 1 ends\n");
	}

	else{
		printf("Process %i begins \n",prcNum);
		//create a pipe
		int fd[2];
		if (pipe(fd) <0 ) {
            printf("Error when creating pipe\n");
			exit(1);
        }

		//fork to start another process and after, output the current process
		int pid = fork();
		if (pid == 0){

			/* when a child is created, the assignment requires to first create a pipe and attach the writing end of the pipe 
			to the standard output of the child  before executing the "cpr num-1" command */
			
			close(fd[0]);//According to https://tldp.org/LDP/lpg/node11.html, if the parent wants to receive data from the child, the child should close fd0
			dup2(fd[0], 1);

			//now we want to  to create a "cpr num-1" process using execvp
            char prcMinus1[128];
            sprintf(prcMinus1, "%i", prcNum - 1);
            char* args[] = {"./cpr", prcMinus1, NULL};
            execvp("./cpr", args);
		}
        close(fd[1]);//According to https://tldp.org/LDP/lpg/node11.html, if the parent wants to receive data from the child, parent should close fd1

		//All processes that create children read from the read end of the pipe and write any data read to their standard output
		char buffer[128]; //create a buffer for the pipe reading and writing
		int reader;//while loop condition
		while((reader = read(fd[0], buffer, 1)) > 0) {//read returns 0 when the file end is reached, thus it will keep going if it returns positive int 
            write(fd[1], buffer, reader);
        }
		/*A prcNum>1 child has finished its process but the parent has not so we must
		sleep 10 seconds to observe zombie processes (unsure if it's correct to put here)*/
        sleep(10);
        printf("Process %i ends\n", prcNum);
	}

}



