/*
 * @author Elnaz Chaposhlo
 *
 * Dice Game with Clearing Shared Memory
*/
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <string.h>
#include <sys/signal.h>


void action(){};

void player(char *name, int playerId, int *sharedArray){

	int points = 0;
        int dice;
        long int ss = 0;

	while(1){
	         signal(SIGUSR1, action); //send a message
		 pause(); //wait until parent send a signal to it

		 printf("%s: Playing my dice\n", name);

	         dice =(int) time(&ss)%10 + 1;
     		 printf("%s: Got %d points\n", name, dice);
	         points += dice;
		 sharedArray[playerId] = points;

		 sleep(2);
                 kill(getppid(), SIGUSR1); //send a message to parent that I am done so that parent$

		}
}


int main(int argc, char *argv[]){
	pid_t pid1, pid2, pid3; 
  	printf("This is a 3-player game with a referee\n");

	int SharedSegment;

	SharedSegment = shmget(IPC_PRIVATE, 3*sizeof(int), IPC_CREAT | 0600); //Get a shared segment
	if(SharedSegment < 0){ //check for fail
	  printf("Failure on shmget.\n");
	  exit(1);
	}

	int *sharedArray;
	sharedArray = shmat(SharedSegment, NULL, 0); // Get a shared array
	if((long int) sharedArray == -1){ //check for fail
	   printf("Failure on shmat.\n");
	   exit(1);
	}


        if((pid1=fork()) == 0)
    		 player("TATA", 0, sharedArray);

        if((pid2=fork()) == 0)
    		 player("TITI", 1, sharedArray);

        if((pid3=fork()) == 0)
     		 player("TOTO", 2, sharedArray);

	sleep(1);
	signal(SIGUSR1, action);


	while(1){
        printf("\nReferee: TATA plays\n\n");
        kill(pid1, SIGUSR1); //send message to process id1 to start
        pause(); //wait until the process finish its job
        printf("\nReferee: Total scores of TATA so far: %d\n\n", sharedArray[0]);


        printf("\nReferee: TITI plays\n\n");
        kill(pid2, SIGUSR1);
        pause();
        printf("\nReferee: Total scores of TITI so far: %d\n\n", sharedArray[1]);

        printf("\nReferee: TOTO plays\n\n");
        kill(pid3, SIGUSR1);
	pause();
        printf("\nReferee: Total scores of TOTO so far: %d\n\n", sharedArray[2]);


	if(sharedArray[0] >= 50){
	   printf("\nReferee:Game is over, TATA won with total score: %d\n\n", sharedArray[0]);
	   int DestroySharedMemory;
        DestroySharedMemory =  shmctl(SharedSegment, IPC_RMID, NULL); //destroy shared memory at th$

         if(DestroySharedMemory == -1){ //check for fail
           printf("Failure on shmctl.\n");
           }else if(DestroySharedMemory == 0){
                printf("Destroying of shared memory was successful.\n\n");
           exit(1);
          }
	   kill(0, SIGTERM);

	}else if(sharedArray[1] >= 50){
           printf("\nReferee:Game is over, TITI won with total score: %d\n\n", sharedArray[1]);
           int DestroySharedMemory;
        DestroySharedMemory =  shmctl(SharedSegment, IPC_RMID, NULL); //destroy shared memory at th$

         if(DestroySharedMemory == -1){ //check for fail
           printf("Failure on shmctl.\n");
           }else if(DestroySharedMemory == 0){
                printf("Destroying of shared memory was successful.\n\n");
           exit(1);
          }
	   kill(0, SIGTERM);

	}else if(sharedArray[2] >= 50){
           printf("\nReferee:Game is over, TOTO won with total score: %d\n\n", sharedArray[2]);
           int DestroySharedMemory;
        DestroySharedMemory =  shmctl(SharedSegment, IPC_RMID, NULL); //destroy shared memory at th$

         if(DestroySharedMemory == -1){ //check for fail
           printf("Failure on shmctl.\n");
           }else if(DestroySharedMemory == 0){
                printf("Destroying of shared memory was successful.\n\n");
           exit(1);
          }
	   kill(0, SIGTERM);
	}

	}
	exit(0);
}

