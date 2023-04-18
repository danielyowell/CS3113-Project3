#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

int sum = 0;

/* Increments sum by 1000 */
void* my_thread_function(void* arg) {
    printf("Beginning thread\n");
    for(int x = 0; x < 1000; x++) {
        sum = sum + 1;
    }
    printf("Ending thread: current sum: %d\n", sum);
    return NULL;
}

/* This program returns a number somewhere between 0 and 10000,
   depending on how many threads have returned by the end of main. */
int main() {
    pthread_t thread_id;
    /* creating threads */
    printf("Creating thread1 from main\n");
    pthread_create(&thread_id, NULL, my_thread_function, NULL);
    printf("Creating thread2 from main\n");
    pthread_create(&thread_id, NULL, my_thread_function, NULL);
    printf("Creating thread3 from main\n");
    pthread_create(&thread_id, NULL, my_thread_function, NULL);
    printf("Creating thread4 from main\n");
    pthread_create(&thread_id, NULL, my_thread_function, NULL);
    printf("Creating thread5 from main\n");
    pthread_create(&thread_id, NULL, my_thread_function, NULL);
    printf("Creating thread6 from main\n");
    pthread_create(&thread_id, NULL, my_thread_function, NULL);
    printf("Creating thread7 from main\n");
    pthread_create(&thread_id, NULL, my_thread_function, NULL);
    printf("Creating thread8 from main\n");
    pthread_create(&thread_id, NULL, my_thread_function, NULL);
    printf("Creating thread9 from main\n");
    pthread_create(&thread_id, NULL, my_thread_function, NULL);
    printf("Creating thread10 from main\n");
    pthread_create(&thread_id, NULL, my_thread_function, NULL);

    printf("Waiting for thread1 completion\n");
    void* thread_return_value1;
    pthread_join(thread_id, &thread_return_value1);
    printf("thread1 done\n");
    printf("Waiting for thread2 completion\n");
    void* thread_return_value2;
    pthread_join(thread_id, &thread_return_value2);
    printf("thread2 done\n");
    printf("Waiting for thread3 completion\n");
    void* thread_return_value3;
    pthread_join(thread_id, &thread_return_value3);
    printf("thread3 done\n");
    printf("Waiting for thread4 completion\n");
    void* thread_return_value4;
    pthread_join(thread_id, &thread_return_value4);
    printf("thread4 done\n");
    printf("Waiting for thread5 completion\n");
    void* thread_return_value5;
    pthread_join(thread_id, &thread_return_value5);
    printf("thread5 done\n");
    printf("Waiting for thread6 completion\n");
    void* thread_return_value6;
    pthread_join(thread_id, &thread_return_value6);
    printf("thread6 done\n");
    printf("Waiting for thread7 completion\n");
    void* thread_return_value7;
    pthread_join(thread_id, &thread_return_value7);
    printf("thread7 done\n");
    printf("Waiting for thread8 completion\n");
    void* thread_return_value8;
    pthread_join(thread_id, &thread_return_value8);
    printf("thread8 done\n");
    printf("Waiting for thread9 completion\n");
    void* thread_return_value9;
    pthread_join(thread_id, &thread_return_value9);
    printf("thread9 done\n");
    printf("Waiting for thread10 completion\n");
    void* thread_return_value10;
    pthread_join(thread_id, &thread_return_value10);
    printf("thread10 done\n");

    printf("Final sum: %d\n", sum);

    return 0;
}