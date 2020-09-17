union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

int sem_create(key_t );
int set_sem(int );
void sem_del(int );
int sem_up(int );
int sem_down(int );