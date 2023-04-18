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
//
#include <pthread.h>

#define BUFFER_SIZE 15

typedef struct {
    // formerly "int buffer[BUFFER_SIZE];"
    char buffer[BUFFER_SIZE];
    int index_read;
    int index_write;
    int count;
    pthread_mutex_t lock;
} circular_buffer;

void circular_buffer_init(circular_buffer *cb) {
    cb->index_read = 0;
    cb->index_write = 0;
    cb->count = 0;
    pthread_mutex_init(&cb->lock, NULL);
}

void circular_buffer_destroy(circular_buffer *cb) {
    pthread_mutex_destroy(&cb->lock);
}

int circular_buffer_empty(circular_buffer *cb) {
    return cb->count == 0;
}

int circular_buffer_full(circular_buffer *cb) {
    return cb->count == BUFFER_SIZE;
}

char circular_buffer_read(circular_buffer *cb) {
    char value;
    pthread_mutex_lock(&cb->lock);
    if (circular_buffer_empty(cb)) {
        value = '\0';
    } else {
        value = cb->buffer[cb->index_read];
        cb->index_read = (cb->index_read + 1) % BUFFER_SIZE;
        cb->count--;
    }
    pthread_mutex_unlock(&cb->lock);
    return value;
}

int circular_buffer_write(circular_buffer *cb, char value) {
    pthread_mutex_lock(&cb->lock);
    if (circular_buffer_full(cb)) {
        return -1;
    } else {
        cb->buffer[cb->index_write] = value;
        cb->index_write = (cb->index_write + 1) % BUFFER_SIZE;
        cb->count++;
    }
    pthread_mutex_unlock(&cb->lock);
    return 0;
}

//

/*
THREAD BEHAVIORS
*/
void* readfile_writebuffer(void* arg) {
    // Do some task in thread 1
    printf("Thread 1\n");
    circular_buffer_write(&cb, 'a');
    circular_buffer_write(&cb, 'b');
    printf("done\n");
    pthread_exit(NULL);
}

void* readbuffer_writeoutput(void* arg) {
    // Do some task in thread 2
    printf("Thread 2\n");
    pthread_exit(NULL);
}

int main(void) {

    printf("hello world\n");
    // CREATE SHARED MEMORY
    circular_buffer cb;
    // int shm_fd;  // File descriptor for shared memory object
    char *shm_ptr;  // Pointer to shared memory object
    // do we still need shared memory objects for threads?

    
    pthread_t thread1, thread2;
    int ret1, ret2;

    // Create thread 1
    ret1 = pthread_create(&thread1, NULL, readfile_writebuffer, NULL);
    if (ret1 != 0) {
        printf("Error creating thread 1\n");
        return -1;
    }

    // Create thread 2
    ret2 = pthread_create(&thread2, NULL, readbuffer_writeoutput, NULL);
    if (ret2 != 0) {
        printf("Error creating thread 2\n");
        return -1;
    }

    // Wait for the threads to finish
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    return 0;
}