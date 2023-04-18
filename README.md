# CS3113-Project3

Daniel Yowell\
Dr. Fang\
CS3113\
4/23/2023

This project solves the producer-consumer problem between threads using a circular buffer and semaphores. It extracts data from a `.dat` file and prints it one-by-one to the console.

To run this program:

1. Download and extract the `.zip` file. 
2. In the command-line interface of your choice, navigate to the `CS3113-Project3` folder. This folder should contain `project3.c`.
3. Enter the command `gcc project3.c -lpthread -lrt` (or `gcc -pthread -o project3 project3.c`). This will compile `project3.c` and generate the executable file as `project3`.
4. Finally, run the executable file by entering the command `./project3`.
