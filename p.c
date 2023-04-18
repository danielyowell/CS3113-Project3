#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <fcntl.h>
#include <sys/sem.h>
#include <sys/ipc.h>

// * SHARED MEMORY

/* key number */
#define SHMKEY ((key_t) 1497)
/* defines struct shared_mem with int variable "value" */
typedef struct
{
    int value;
} shared_mem;
/* create shared memory (pointer) */
shared_mem *total;

// * SEMAPHORES

/* semaphore key */
#define SEMKEY ((key_t) 400L)

/* number of semaphores being created */
#define NSEMS 1

/* GLOBAL */
int semaphore_id; /* semaphore id */

/* semaphore buffers */
static struct sembuf OP = {0,-1,SEM_UNDO};
static struct sembuf OV = {0,1,SEM_UNDO};

/* union to store semaphore value (I think struct also works) */
typedef union {
  int val;
  struct semid_ds *buf;
  ushort *array;
} semunion;

/* function for semaphore to protect critical section */
int POP(){
return semop(semaphore_id, &OP,1);
}

/* function for semaphore to release protection */
int VOP(){
return semop(semaphore_id, &OV,1);
}

int producer() {
    do {
        ...
        // produce an item into nextp
        ...
        mutex lock(mutex);
        while(full)
        condition wait(nonfull, mutex);
        ...
        // add nextp to buffer
        ...
        condition signal(nonempty);
        mutex unlock(mutex);
    } while(TRUE)
}

int consumer() {
    do {
        mutex lock(mutex);
        while(empty)
        condition wait(nonempty, mutex);
        ...
        // remove an item from the buffer to nextc
        ...
        condition signal(nonfull);
        mutex unlock(mutex);
        ...
        // consume the item in nextc
        ...
    } until(FALSE)
}
int main(void) {
      return 0;
  }
