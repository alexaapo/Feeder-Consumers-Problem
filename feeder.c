#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/timeb.h>
#include <math.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <semaphore.h>
#include <string.h>
#include "shared_memory.h"
#include "semaphores.h"

//Declaration of 2 dynamic arrays of semaphores
//in order to each process has her own 2-dinary semaphores.
unsigned int *semaphore_1, *semaphore_2;

int main(int argc, char* argv[])
{
    int M, n, pid, status, pos;
    struct timeval current_time;
    long int delay, sum_delays = 0, average_delay;

    if(argc !=3){
        printf("Error! You have to insert 2 arguments! \n");
        exit(1);
    }
    else{

        M = atoi(argv[1]);

        if(M <= 3000)
        {
            printf("Error! M must be greater than 3000 \n");
            exit(1);
        }

        n = atoi(argv[2]);

        if(n <= 0)
        {
            printf("Error! n must be positive number \n");
            exit(1);
        }
    }

    int array_of_integers[M];

    //Seed the random number generator
    srand(time(0));

    //Fill the array with random numbers in range of [0,99]
    int i;
    for (i = 0; i < M; i++) 
        array_of_integers[i] = rand()%100;

    //Allocate memory for the 2 arrays of binary semaphores.
    unsigned int* semaphore_1 = (unsigned int*)malloc(n*sizeof(unsigned int));
    unsigned int* semaphore_2 = (unsigned int*)malloc(n*sizeof(unsigned int));

    //Give to semaphores their sem_id's
    for(i=0;i<n;i++)
    {
        semaphore_1[i] = sem_create((key_t)rand()%1000000000);
        semaphore_2[i] = sem_create((key_t)rand()%1000000000);
    }

    key_t key;

    //Producing a random key for shared memory.
    if ((key = ftok("feeder.c", 'R')) == -1) 
    {                
        printf("Error! Cannot create key!   \n");
        exit(1);
    }

    //Allocate memory for the shared memory.
    //It must have only one integer and its timestamp;
    int shm_size = 1*sizeof(Entry);
    int shm_id = shmcreate(key, shm_size);
    Entry* shm_ptr = shmattach(shm_id);

    //Initialize the integer of shared memory
    shm_ptr->number = 0;

    //Create n child processes
    for(i=0; i<n; i++) 
    {                     
        pid = fork();
        pos = i;
        if(pid == -1)
        {
            printf("Fork failed!\n");
            return -1;
        }
        if(pid == 0)  //If its child, do not fork!
            break;
    }

    //In case of child process
    if(pid == 0)
    {
        //This is the copied-array of integers of each process
        int array_of_process[M];

        for(int i=0;i<M;i++)
        {
            //Block the processes
            if(!(sem_down(semaphore_1[pos])))  exit(1);

            //Ci process take the integer from shared memory.
            array_of_process[i] = shm_ptr->number;

            //Take the current time.
            gettimeofday(&current_time,NULL);

            //Calculate the delay of each Ci process.
            delay = ((current_time.tv_sec-shm_ptr->timestamp.tv_sec)*1000000L) + (current_time.tv_usec-shm_ptr->timestamp.tv_usec);

            //Sum all the delays, so to calculate the average delay later.
            sum_delays+=delay;

            //Unblock processes
            if(!(sem_up(semaphore_2[pos])))  exit(1);
        }

        //Calculate the average delay and print it on terminal.
        average_delay = sum_delays/M;
        printf("The average delay of process %d is %ld ms.\n", getpid(), average_delay);

        //Each process creates a file with its array of integers, its pid and its average delay.
        char filename[20], pid[20];
        
        sprintf(pid, "%d", getpid());
        strcpy(filename,"file");
        strcat(filename,pid);

        //Open the file.
        FILE* file_ptr = fopen(filename,"w");

        if(file_ptr == NULL)
        {
            printf("Error with creation of the file.\n");
            exit(EXIT_FAILURE);
        }

        //Save in the files the appropriate information.
        fprintf(file_ptr,"The array of integers of process %d is: \n",getpid());

        for(i=0;i<M;i++)
            fprintf(file_ptr, "%d ", array_of_process[i]);
        
        fprintf(file_ptr, "\nThe average delay of process %d is: %ld ms\n", getpid(), average_delay);

        //CLose file
        fclose(file_ptr);

        //Exit of process in order to terminate.
        exit(1);
    }
    else //In case of parent process
    {
        for(i=0;i<M;i++)
        {
            //Put the integer to shared memory.
            shm_ptr->number = array_of_integers[i];

            //Save the current time to timestamp of shared memory.
            gettimeofday(&shm_ptr->timestamp,NULL);

            for(int j=0;j<n;j++)
            {
                //Unblock all the processes
                if(!(sem_up(semaphore_1[j])))  exit(1);
            }

            //Block all the processes except the last one.
            if(i!=(M-1))
            {
                for(int j=0;j<n;j++)
                {
                    if(!(sem_down(semaphore_2[j])))  exit(1);
                }
            }
        }

        //Parent process wait until all processes have terminated.
        while(wait(&status)>0);
    }

    //Detach shared memory
    shmdetach(shm_ptr);
    shmdelete(shm_id);

    //Delete semaphores' arrays
    for(int i=0;i<n;i++)
    {
        sem_del(semaphore_1[i]);
        sem_del(semaphore_2[i]);
    }

    free(semaphore_1);
    free(semaphore_2);
}