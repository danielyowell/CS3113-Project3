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
    char buffer[BUFFER_SIZE];
    int head;
    int tail;
    pthread_mutex_t mutex;
    sem_t full;
    sem_t empty;
} circular_buffer;

void circular_buffer_init(circular_buffer *cb) {
    cb->head = 0;
    cb->tail = 0;
    pthread_mutex_init(&cb->mutex, NULL);
    sem_init(&cb->full, 0, BUFFER_SIZE);
    sem_init(&cb->empty, 0, 0);
}

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

void *write_to_buffer(void *arg) {
    circular_buffer *cb = (circular_buffer *) arg;
    FILE *fp;
    char c;
    fp = fopen("data.dat", "r");
    if (fp == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    while ((c = fgetc(fp)) != EOF) {
        if (c == '*') {
            break;
        }
        if (circular_buffer_write(cb, c) != 0) {
            fprintf(stderr, "Circular buffer is full\n");
            break;
        }
    }
    fclose(fp);
    return NULL;
}

void *read_from_buffer(void *arg) {
    circular_buffer *cb = (circular_buffer *) arg;
    char c;
    while ((c = circular_buffer_read(cb)) != '*') {
        putchar(c);
        sleep(1);
    }
    return NULL;
}

int main() {
    pthread_t write_thread, read_thread;
    circular_buffer cb;
    circular_buffer_init(&cb);
    pthread_create(&write_thread, NULL, write_to_buffer, &cb);
    pthread_create(&read_thread, NULL, read_from_buffer, &cb);
    pthread_join(write_thread, NULL);
    pthread_join(read_thread, NULL);
    return 0;
}
