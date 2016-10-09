#include <unistd.h>     /* Symbolic Constants */
#include <sys/types.h>  /* Primitive System Data Types */ 
#include <errno.h>      /* Errors */
#include <stdio.h>      /* Input/Output */
#include <sys/wait.h>   /* Wait for Process Termination */
#include <stdlib.h>     /* General Utilities */

int A_to_B[2], B_to_A[2];

int main(){
  int status;

  pipe(A_to_B);
  pipe(B_to_A);
  int i=0,j=0;
  if(fork()==0){
    printf("IN PA\n");
    close(A_to_B[0]);
    for(i=0; i<10; i++){
      int x = i*5, y = 0;
      write(A_to_B[1], (void *)&x, sizeof(int));
      sleep(1);
      read(B_to_A[0], (void *)&y, sizeof(int));
      printf("PA: x=%d, y:%d \n",x, y);
    }
    return 0;
  }
  if(fork()==0){
    printf("IN PB\n");
    close(A_to_B[1]);
    for(j=0; j<10; j++){
      int x = 0, y = j*5;
      sleep(1);
      read(A_to_B[0], (void *)&x, sizeof(int));
      write(B_to_A[1], (void *)&y, sizeof(int));
      printf("PB: x=%d, y:%d \n",x, y);
    }
    return(0);
  }
  wait(&status);
  wait(&status);
  printf("END\n");
}
