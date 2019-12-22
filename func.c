#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

/* semaphore POSIX */
#include <semaphore.h>

#define SHARED_KEY 1014
#define SIZE_BUF 512

// int shm_id = -1;

typedef struct {
    char buf[SIZE_BUF];
    int left;
    int right;
} buffer;

typedef struct shared_data {
    buffer mes;
    
    pid_t prov_id;
    pid_t cons_id;
    
    sem_t empty;
    sem_t full;
} shared;



FILE* file = NULL;

void openFile() {
    file = fopen("text.txt", "r");
}

void closeFile() {
    fclose(file);
}
