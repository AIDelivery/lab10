#include "func.c"
#include "errno.h"
#include "string.h"
#include <iostream>
using namespace std;

int sem_f = 0, sem_e = 0;
shared* sharedContent;

bool provider_is_dead = false;
void registr (int sig) {
    printf("\n\n[Provider terminated]\n\n");
    
    provider_is_dead = true;
    sem_getvalue(&(sharedContent->full), &sem_f);
    sem_getvalue(&(sharedContent->empty), &sem_e);
    
    if(sem_f == 0) {
        printf("\n\t- - - End of message - - -\n");
        exit(EXIT_SUCCESS);
    }
}


int main(int argc, char* argv[]) {
    int shm_id = shmget(SHARED_KEY, sizeof(shared_data), (IPC_CREAT | 0666));
    void* shmaddr = shmat(shm_id, NULL, 0);
    sharedContent = (shared*) shmaddr;
    char ch;
    
    sharedContent->cons_id = getpid();
    sigset_t ss; sigemptyset(&ss); sigaddset(&ss, SIGQUIT); sigprocmask(SIG_UNBLOCK, &ss, NULL);
    signal(SIGQUIT, registr);

    printf("\t- - - Message from provider - - -\n");
    
    while (true) {
        sem_wait(&(sharedContent->full));
        
        fflush(stdout);
        ch = sharedContent->mes.buf[sharedContent->mes.right];
        printf("%c", ch);
        sharedContent->mes.right = (sharedContent->mes.right + 1) % SIZE_BUF;
        
        sem_post(&(sharedContent->empty));
        
        sem_getvalue(&(sharedContent->full), &sem_f);
        if(provider_is_dead && sem_f == 0) break;
    }
    
    printf("\n\t- - - End of message - - -\n");

    sem_destroy(&(sharedContent->empty));
    sem_destroy(&(sharedContent->full));
    shmctl(shm_id, IPC_RMID, NULL);

    return 0;
}
