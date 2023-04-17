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

// Define a struct for the circular buffer with the following fields:
// - An array to store the data
// - A size to indicate the size of the buffer
// - A read index to track the next item to read
// - A write index to track the next item to write
// - A count to track the number of items in the buffer
typedef struct {
    int* buffer;
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
int circular_buffer_write(CircularBuffer* cb, int item) {
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
int circular_buffer_read(CircularBuffer* cb, int* item) {
    if (cb->count == 0) {
        return -1; // buffer empty
    }
    *item = cb->buffer[cb->read_index];
    cb->read_index = (cb->read_index + 1) % cb->size;
    cb->count--;
    return 0;
}

// Write a function to free the memory used by the circular buffer.
void circular_buffer_free(CircularBuffer* cb) {
    free(cb->buffer);
}

int main(void) {
    printf("hello world\n");
    CircularBuffer cb;
    circular_buffer_init(&cb, 5);
    circular_buffer_write(&cb, 1);
    circular_buffer_write(&cb, 2);
    int item;
    circular_buffer_read(&cb, &item); // item = 1
    circular_buffer_write(&cb, 3);
    circular_buffer_write(&cb, 4);
    circular_buffer_write(&cb, 5);
    circular_buffer_write(&cb, 6); // error: buffer full
    circular_buffer_read(&cb, &item); // item = 2
    circular_buffer_read(&cb, &item); // item = 3
    circular_buffer_read(&cb, &item); // item = 4
    circular_buffer_read(&cb, &item); // item = 5
    circular_buffer_read(&cb, &item); // error: buffer empty
    circular_buffer_free(&cb);
    printf("successful circlular buffer\n");
}