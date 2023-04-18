// @author Daniel Yowell
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
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

//? HELPER METHODS

bool circular_buffer_has_space(circular_buffer *cb) {
    bool result;
    pthread_mutex_lock(&cb->mutex);
    int next_tail = (cb->tail + 1) % BUFFER_SIZE;
    result = (next_tail != cb->head);
    pthread_mutex_unlock(&cb->mutex);
    return result;
}

// write to buffer (do NOT print)
int circular_buffer_write(circular_buffer *cb, char c) {
    sem_wait(&cb->full);
    pthread_mutex_lock(&cb->mutex);
    int next_tail = (cb->tail + 1) % BUFFER_SIZE;
    if (next_tail == cb->head) {
        pthread_mutex_unlock(&cb->mutex);
        sem_post(&cb->full);
        return -1; // Buffer is full
    }
    cb->buffer[cb->tail] = c;
    cb->tail = next_tail;
    pthread_mutex_unlock(&cb->mutex);
    sem_post(&cb->empty);
    return 0;
}

// extract character from buffer, move up head of buffer
char circular_buffer_read(circular_buffer *cb) {
    // wait on the "empty" semaphore to ensure that there is at least one slot occupied in the buffer
    sem_wait(&cb->empty);
    // acquire mutex
    pthread_mutex_lock(&cb->mutex);
    // CRITICAL SECTION
        // Buffer is empty
        if (cb->head == cb->tail) {
            pthread_mutex_unlock(&cb->mutex);
            sem_post(&cb->empty);
            return '\0'; 
        }
        // Otherwise, extract character from head of buffer and print
        char c = cb->buffer[cb->head];
        // update head index
        cb->head = (cb->head + 1) % BUFFER_SIZE;
    // END CRITICAL SECTION
    // release mutex
    pthread_mutex_unlock(&cb->mutex);
    // signal the "full" semaphore to indicate that there is one more slot available in the buffer
    sem_post(&cb->full);
    return c;
}

//! BIG BOYS

/*
THREAD 1: PRODUCER

While the file is not empty, check if there is space in the buffer.
    If there is space, read a character from the file and add it to the buffer.
*/
void* readfile_writebuffer(void* arg) {
    // set up circular buffer
    circular_buffer *cb = (circular_buffer *) arg;
    char c = 'x';

    // open file
    FILE *fp;
    fp = fopen("mytest.dat", "r");
    if (fp == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    // while the end of the file has not been reached
    while (c != EOF) {
        // check for space in buffer
        bool x = circular_buffer_has_space(cb);
        if(x == true) {
            c = fgetc(fp);
            circular_buffer_write(cb, c);
        }
    }
    // wait until there is space for one more character
    while(circular_buffer_has_space(cb) == false) {

    }
    // write closing character '*' to buffer
    circular_buffer_write(cb, '*');
    
    // close file
    fclose(fp);
    pthread_exit(NULL);
    // printf("exiting producer\n");
}

// CONSUMER
void* readbuffer_writeoutput(void* arg) {
    circular_buffer *cb = (circular_buffer *) arg;
    char c;
    while(true) {
        usleep(50000); // sleep(1);
        c = circular_buffer_read(cb);
        if(c == '*') {
            pthread_exit(NULL);
        }
        printf("%c", c);
        fflush(stdout);
    }
    printf("exiting consumer\n");
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
    printf("\njoined threads\n");
    return 0;
}