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

// write to output
char circular_buffer_read(circular_buffer *cb) {
    sem_wait(&cb->empty);
    pthread_mutex_lock(&cb->mutex);
    // CRITICAL SECTION
        // Buffer is empty
        if (cb->head == cb->tail) {
            pthread_mutex_unlock(&cb->mutex);
            sem_post(&cb->empty);
            return '\0'; 
        }
        char c = cb->buffer[cb->head];
        printf("%c\n",c);
        cb->head = (cb->head + 1) % BUFFER_SIZE;
    // END CRITICAL SECTION
    pthread_mutex_unlock(&cb->mutex);
    sem_post(&cb->full);
    return c;
}

// write to buffer (do not print)
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

// READ FROM FILE, WRITE TO BUFFER
void *write_to_buffer(void *arg) {
    // this should be the same circular buffer as before (arg)
    circular_buffer *cb = (circular_buffer *) arg;
    FILE *fp;
    char c;

    // open file
    fp = fopen("mytest.dat", "r");
    if (fp == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    // while the end of the file has not been reached
    while ((c = fgetc(fp)) != EOF) {
        if (c == '*') {
            break;
        }
        // write the character to the buffer, unless it is 0
        if (circular_buffer_write(cb, c) != 0) {
            //fprintf(stderr, "Circular buffer is full\n");
            //break;
        }
    }
    fclose(fp);
    return NULL;
}

// READ FROM BUFFER, WRITE TO OUTPUT
// after writing to output, remove from buffer
void *read_from_buffer(void *arg) {
    circular_buffer *cb = (circular_buffer *) arg;
    char c;
    while ((c = circular_buffer_read(cb)) != '*') {
        //putchar(c);
        //printf("%c",c);
        sleep(1);
    }
    return NULL;
}

// MAIN FUNCTION
int main() {
    // Create pthread objects
    pthread_t write_thread, read_thread;
    
    // Initialize circular buffer
    circular_buffer cb;
    circular_buffer_init(&cb);

    // CREATE PRODUCER: reads from file, writes to buffer
    // we pass &cb as an argument to write_to_buffer. does this mean it gets used by the thread?
    pthread_create(&write_thread, NULL, write_to_buffer, &cb);
    // CREATE CONSUMER: reads from buffer, writes to output
    pthread_create(&read_thread, NULL, read_from_buffer, &cb);
    
    /*
    circular_buffer_write(&cb, 'a');
    circular_buffer_write(&cb, 'm');
    circular_buffer_write(&cb, 'o');
    circular_buffer_write(&cb, 'g');
    circular_buffer_write(&cb, 'u');
    circular_buffer_write(&cb, 's');
    
    circular_buffer_read(&cb);
    circular_buffer_read(&cb);
    circular_buffer_read(&cb);
    circular_buffer_read(&cb);
    circular_buffer_read(&cb);
    circular_buffer_read(&cb);    
    */

    pthread_join(write_thread, NULL);
    pthread_join(read_thread, NULL);
    return 0;
}
