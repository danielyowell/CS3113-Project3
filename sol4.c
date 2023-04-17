// clean slate
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
#include <pthread.h>
#include <semaphore.h>

// STRUCT
#define BUFFER_SIZE 15
typedef struct {
    char buffer[BUFFER_SIZE];
    int head;
    int tail;
    pthread_mutex_t mutex;
    sem_t full;
    sem_t empty;
} circular_buffer;

// INITIALIZE CIRCULAR BUFFER
void circular_buffer_init(circular_buffer *cb) {
    cb->head = 0;
    cb->tail = 0;
    pthread_mutex_init(&cb->mutex, NULL);
    sem_init(&cb->full, 0, BUFFER_SIZE);
    sem_init(&cb->empty, 0, 0);
}

// ! READ FROM [FILE], WRITE TO [BUFFER]
void *write_to_buffer(void *arg) {
    printf("beginning write_to_buffer\n");
    // this should be the same circular buffer as before (arg)
    circular_buffer *cb = (circular_buffer *) arg;
    FILE *fp;
    char c;
    printf("exiting write_to_buffer\n");
    fclose(fp);
    return NULL;
}
// ! READ FROM [BUFFER], [WRITE TO OUTPUT]
// after writing to output, remove from buffer
void *read_from_buffer(void *arg) {
    printf("beginning read_from_buffer\n");
    circular_buffer *cb = (circular_buffer *) arg;
    char c;
    /**/
    printf("exiting read_from_buffer\n");
    return NULL;
}

void* thread1_func(void* arg) {
    // Do some task in thread 1
    printf("Thread 1\n");
    pthread_exit(NULL);
}
void* thread2_func(void* arg) {
    // Do some task in thread 2
    printf("Thread 2\n");
    pthread_exit(NULL);
}

// MAIN FUNCTION
int main() {
    // Create pthread objects
    pthread_t write_thread, read_thread;
    
    // Initialize circular buffer
    circular_buffer cb;
    circular_buffer_init(&cb);

    // CREATE PRODUCER: reads from file, writes to buffer
    pthread_create(&write_thread, NULL, thread1_func, NULL); // &cb arg

    // CREATE CONSUMER: reads from buffer, writes to output
    pthread_create(&read_thread, NULL, thread2_func, NULL);

    pthread_join(write_thread, NULL);
    pthread_join(read_thread, NULL);
    printf("threads have been joined\n");
    return 0;
}