#include "func.c"
#include "errno.h"
#include "string.h"
#include <iostream>
using namespace std;

bool provider_is_dead = false;
void registr (int sig) { provider_is_dead = true; }


int main(int argc, char* argv[]) {
    int shm_id = shmget(SHARED_KEY, sizeof(shared_data), (IPC_CREAT | 0666));
    void* shmaddr = shmat(shm_id, NULL, 0);
    shared* sharedContent = (shared*) shmaddr;
    int sem_f = 0, sem_e;
    char ch;
    
    sharedContent->cons_id = getpid();
    sigset_t ss; sigemptyset(&ss); sigaddset(&ss, SIGQUIT); sigprocmask(SIG_UNBLOCK, &ss, NULL);
    signal(SIGQUIT, registr);

    printf("\t- - - Message from provider - - -\n");
    
    while (true) {cout << sem_f << ' ' << provider_is_dead << endl;
        sem_getvalue(&(sharedContent->full), &sem_f);
        sem_getvalue(&(sharedContent->empty), &sem_e);
        if (provider_is_dead && sem_f == 0)
            break;
        else
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
