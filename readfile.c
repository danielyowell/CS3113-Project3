#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE* fp = fopen("mytest.dat", "r");
    if (fp == NULL) {
        printf("Error opening file\n");
        return -1;
    }
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char* buffer = malloc(file_size);
    size_t bytes_read = fread(buffer, 1, file_size, fp);
    if (bytes_read < file_size) {
        printf("Error reading file\n");
        return -1;
    }
    fclose(fp);
    // process the contents of the file as needed
    free(buffer);
    return 0;
}
