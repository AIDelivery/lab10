#include "func.c"
#include <iostream>
using namespace std;

int main(int argc, char* argv[]) {
    int shm_id = shmget(SHARED_KEY, sizeof(shared_data), (IPC_CREAT | 0666));
    void* shmaddr = shmat(shm_id, NULL, 0);
    shared* sharedContent = (shared*) shmaddr;
    int sem_f = 0, sem_e = 0;
    char ch;
    
    sharedContent->prov_id = getpid();
    sem_init(&(sharedContent->full), 1, 0);
    sem_init(&(sharedContent->empty), 1, SIZE_BUF);
    sharedContent->mes.left = 0;
    sharedContent->mes.right = 0;
    
    openFile();

    while (true) {
        sem_wait(&(sharedContent->empty));
        
        ch = fgetc(file);
        if (ch == EOF) break;
        
        sharedContent->mes.buf[sharedContent->mes.left] = ch;
        sharedContent->mes.left = (sharedContent->mes.left + 1) % SIZE_BUF;
        
//         usleep(100);
        sem_post(&(sharedContent->full));
    }
    
    printf("Copying to shared memory was ended\n");
    
    sem_post(&(sharedContent->empty));
    while(!sharedContent->cons_id);
    printf("Signal sending\n");
    kill(sharedContent->cons_id, SIGQUIT);
    
    /*
    while (sem_f != 0) {
        sem_getvalue(&(sharedContent->empty), &sem_e);
        sem_getvalue(&(sharedContent->full), &sem_f);
        
        printf("Empty after EOF: %d\n", sem_e);
        printf("Full after EOF: %d\n", sem_f);
    }
    
    printf("Empty after ALL: %d\n", sem_e);
    printf("Full after ALL: %d\n", sem_f);
    */
    
    printf("Waiting while consumer is ready\n");
    
    while (sem_e != 512) {
        sem_getvalue(&(sharedContent->empty), &sem_e);
        printf("Empty in loop: %d\n", sem_e);
    }
    
    shmctl(shm_id, IPC_RMID, NULL);
    closeFile();
    return 0;
}
