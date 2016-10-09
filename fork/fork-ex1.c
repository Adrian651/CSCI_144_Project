/* Includes */
#include <unistd.h>     /* Symbolic Constants */
#include <sys/types.h>  /* Primitive System Data Types */ 
#include <errno.h>      /* Errors */
#include <stdio.h>      /* Input/Output */
#include <sys/wait.h>   /* Wait for Process Termination */
#include <stdlib.h>     /* General Utilities */
 
int main()
{
    pid_t childpid; /* variable to store the child's pid */
    int retval;     /* child process: user-provided return code */
    int status;     /* parent process: child's exit status */

    /* only 1 int variable is needed because each process would have its
       own instance of the variable
       here, 2 int variables are used for clarity */
        
    /* now create new process */
    int x = 0;
    while(x < 10)
    {
    childpid = fork();
    
    if (childpid >= 0) /* fork succeeded */
    {
        if (childpid == 0) /* fork() returns 0 to the child process */
        {
            int i=0;
            for(i=0;i<10;i++)
            {
               sleep(1); /* sleep for 1 second */
            }
            printf("CHILD%d: Goodbye!\n",x);    
            exit(0); /* child exits with user-provided return code */
        }
        else /* fork() returns new pid to the parent process */
        {
            printf("PARENT: Creating %d-th child process!\n",x);             
            int i;
            for(i=0;i<100;i++)
            {
               usleep(100); /* sleep for 1 second */
            }
        }
    }
    else /* fork returns -1 on failure */
    {
        perror("fork"); /* display error message */
        exit(0); 
    }
    x++;
    }
    wait(&status); /* wait for child to exit, and store its status */
    printf("PARENT: Goodbye!\n");             
    exit(0);
}
