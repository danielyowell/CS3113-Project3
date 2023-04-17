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
    // mutex_lock: safety with semaphores!
    pthread_mutex_lock(&cb->lock);
    // if the buffer is empty
    if (circular_buffer_empty(cb)) {
        value = '\0';
    } else {
        /*
        value = cb->buffer[cb->index_read];
        cb->index_read = (cb->index_read + 1) % BUFFER_SIZE;
        cb->count--;
        */
        value = 
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

void *write_to_buffer(void *arg) {
    circular_buffer *cb = (circular_buffer *)arg;
    char c = 'A';
    while (1) {
        if (circular_buffer_write(cb, c) == 0) {
            printf("Wrote %c to buffer\n", c);
            c = (c + 1) % 26 + 'A';
        }
        sleep(1);
    }
}

void *read_from_buffer(void *arg) {
    circular_buffer *cb = (circular_buffer *)arg;
    while (1) {
        char c = circular_buffer_read(cb);
        if (c != '\0') {
            printf("Read %c from buffer\n", c);
        }
        sleep(1);
    }
}

// READS FROM mytest.dat
void *read_from_file(int x) {
    circular_buffer *cb = (circular_buffer *)arg;
    FILE *file = fopen("mytest.dat", "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file\n");
        return NULL;
    }
    char c = fgetc(file);
    while (c != EOF && c != '*') {
        if (circular_buffer_write(cb, c) == 0) {
            printf("Wrote %c to buffer\n", c);
        }
        c = fgetc(file);
        sleep(1);
    }
    fclose(file);
    return NULL;
}

int main() {
    circular_buffer cb;
    circular_buffer_init(&cb);
    //cb = shm_open("/myshm", O_CREAT | O_RDWR, 0666);

    // Create thread for writing to circular buffer
    /*
    pthread_t write_thread;
    if (pthread_create(&write_thread, NULL, write_to_buffer, (void *)&cb)) {
        fprintf(stderr, "Error creating write thread\n");
        return -1;
    }
    */


    // Create thread for reading from circular buffer
    pthread_t read_thread;
    if (pthread_create(&read_thread, NULL, read_from_file(10), (void *)&cb)) {
        fprintf(stderr, "Error creating read thread\n");
        return -1;
    }

    // Wait for threads to complete
    //pthread_join(write_thread, NULL);
    pthread_join(read_thread, NULL);

    circular_buffer_destroy(&cb);

    return 0;
}

