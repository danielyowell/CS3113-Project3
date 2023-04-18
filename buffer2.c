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
// THREAD TIME
#include <pthread.h>
// SHARED MEMORY
#define SHM_SIZE 1024  // Size of shared memory object



// Define a struct for the circular buffer with the following fields:
// - An array to store the data
// - A size to indicate the size of the buffer
// - A read index to track the next item to read
// - A write index to track the next item to write
// - A count to track the number of items in the buffer
typedef struct {
    char* buffer;
    size_t size;
    size_t read_index;
    size_t write_index;
    size_t count;
} CircularBuffer;

// Write a function to initialize the circular buffer with the given size. 
// This function should allocate memory for the buffer and initialize the other fields.
void circular_buffer_init(CircularBuffer* cb, size_t size) {
    cb->buffer = malloc(sizeof(int) * size);
    cb->size = size;
    cb->read_index = 0;
    cb->write_index = 0;
    cb->count = 0;
}

// Write a function to add an item to the circular buffer. 
// This function should write the item to the buffer at the current write index and increment the write index.
// If the write index reaches the end of the buffer, it should wrap around to the beginning. 
// If the buffer is full, the function should return an error.
int circular_buffer_write(CircularBuffer* cb, char item) {
    if (cb->count == cb->size) {
        return -1; // buffer full
    }
    cb->buffer[cb->write_index] = item;
    cb->write_index = (cb->write_index + 1) % cb->size;
    cb->count++;
    return 0;
}

// Write a function to read an item from the circular buffer. 
// This function should read the item at the current read index and increment the read index. 
// If the read index reaches the end of the buffer, it should wrap around to the beginning. 
// If the buffer is empty, the function should return an error.
int circular_buffer_read(CircularBuffer* cb, char* item) {
    // buffer empty
    if (cb->count == 0) {
        return -1; 
    }
    // 
    *item = cb->buffer[cb->read_index];
    cb->read_index = (cb->read_index + 1) % cb->size;
    cb->count--;
    return 0;
}

// Write a function to free the memory used by the circular buffer.
void circular_buffer_free(CircularBuffer* cb) {
    free(cb->buffer);
}

/*
THREAD BEHAVIORS
*/
void* readfile_writebuffer(void* arg) {
    // Do some task in thread 1
    printf("Thread 1\n");
    pthread_exit(NULL);
}

void* readbuffer_writeoutput(void* arg) {
    // Do some task in thread 2
    printf("Thread 2\n");
    pthread_exit(NULL);
}

int main(void) {
    printf("hello world\n");
    CircularBuffer cb;

    // int shm_fd;  // File descriptor for shared memory object
    char *shm_ptr;  // Pointer to shared memory object

    // Create a new shared memory object with read/write permissions
    cb = shm_open("/myshm", O_CREAT | O_RDWR, 0666);


    circular_buffer_init(&cb, 5);
    circular_buffer_write(&cb, 'a');
    circular_buffer_write(&cb, 'b');
    char item = 'x';
    printf("Current item value: %c\n", item);
    circular_buffer_read(&cb, &item); // item = 1
    printf("Current item value: %c\n", item);
    circular_buffer_write(&cb, 'c');
    circular_buffer_write(&cb, 'd');
    circular_buffer_write(&cb, 'e');
    circular_buffer_write(&cb, 'f'); // error: buffer full
    printf("Current item value: %c\n", item);
    circular_buffer_read(&cb, &item); // item = 2
    printf("Current item value: %c\n", item);
    circular_buffer_read(&cb, &item); // item = 3
    printf("Current item value: %c\n", item);
    circular_buffer_read(&cb, &item); // item = 4
    printf("Current item value: %c\n", item);
    circular_buffer_read(&cb, &item); // item = 5
    printf("Current item value: %c\n", item);
    circular_buffer_read(&cb, &item); // error: buffer empty
    circular_buffer_free(&cb);
    printf("successful circular buffer\n");

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

}