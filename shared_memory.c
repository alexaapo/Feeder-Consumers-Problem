#include "shared_memory.h"

int shmcreate(key_t key, int shm_size){
    int tmp = shmget(IPC_PRIVATE, shm_size, IPC_CREAT | 0666);
    if(tmp < 0){
      printf("Error of segment\n");
      exit(1);
    }
    return tmp;
}

Entry* shmattach(int shm_id){
    Entry* first_entry = (Entry*)shmat(shm_id, (void *)0, 0);
    if(first_entry == (Entry*)(-1))
    {
        printf("* error attach *\n");
        exit(1);
    }
    return first_entry;
}

int shmdetach(Entry* shm_ptr){
    int tmp = shmdt(shm_ptr);
    if(tmp == -1)
    {
        printf("Error with Detach of shared memory!\n");
        exit(1);
    }
    return tmp;
}

int shmdelete(int shm_id){
    int tmp = shmctl(shm_id, IPC_RMID, 0);
    if(tmp == -1)
    {
        printf("Error with Delete of shared memory\n");
        exit(1);
    }
    return tmp;
}

// int shm2detach(int *shm_ptr)
// {
//     int tmp = shmdt(shm_ptr);
//     if(tmp == -1)
//     {
//         printf("Error with Detach of shared memory!\n");
//         exit(1);
//     }
//     return tmp;
// }

// int* shm2attach(int shm_id)
// {
//     int* first = (int*)shmat(shm_id, (void *)0, 0);
//     if(first == (int*)(-1))
//     {
//         printf("* error attach *\n");
//         exit(1);
//     }
//     return first;
// }