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

void* readfile_writebuffer(void* arg) {
    // Do some task in thread 1
    circular_buffer *cb = (circular_buffer *) arg;
    char c;
    printf("Thread 1\n");

    // open file
    FILE *fp;
    fp = fopen("mytest.dat", "r");
    if (fp == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    // close file
    fclose(fp);

    pthread_exit(NULL);
}
void* readbuffer_writeoutput(void* arg) {
    // Do some task in thread 2
    circular_buffer *cb = (circular_buffer *) arg;
    char c;
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
    pthread_create(&write_thread, NULL, readfile_writebuffer, &cb);

    // Sleep for 0.5 seconds
    usleep(100000);
    
    // CREATE CONSUMER: reads from buffer, writes to output
    pthread_create(&read_thread, NULL, readbuffer_writeoutput, &cb);

    pthread_join(write_thread, NULL);
    pthread_join(read_thread, NULL);
    printf("threads have been joined\n");
    return 0;
}