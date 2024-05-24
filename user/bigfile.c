#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

// Параметры для линейной конгруэнтной последовательности
#define A 1664525
#define C 1013904223
#define M 4294967296 // 2^32
#define BUFSIZE 4096

int buf[BUFSIZE];
// Генератор случайных чисел
uint rand_seed = 42;

uint rand() {
    rand_seed = (A * rand_seed + C) % M;
    return rand_seed;
}

int create_random_file(char *filename, int size) {
    int fd = open(filename, O_CREATE | O_WRONLY);
    if (fd < 0) {
        printf("Error creating file\n");
        exit(1);
    }
    int pointer = 0;
    int wsize = 0;
    for (int i = 0; i < size; i += 4) {
        uint random_value = rand();
        buf[pointer++] = random_value;
        if (pointer == BUFSIZE || i == size - 4) {
            int st = write(fd, buf, pointer * 4);
            if (st < 0)
                fprintf(2, "Error writing \n");

            wsize += st;
            printf( "W In process: %d bytes\n", wsize);
            if (st != pointer * 4)
                break;
            pointer = 0;
        }
    }

    close(fd);

    if (wsize != size) {
        printf("Error in writing %d bytes, only %d\n", size, wsize);
        exit(0);
    }
    return wsize;
}

int read_and_verify_file(char *filename, int size) {
    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        printf("Error opening file\n");
        exit(1);
    }
    int rsize = 0;
    int csize = 0;

    for (int i = 0; i < size; i += 4) {
        uint expected_value = rand();
        int st = read(fd, buf, BUFSIZE * 4);
        if (st == 0){
            break;
        } else if (st < 0){
            fprintf(2, "Error reading \n");
        }
        rsize += st;
        printf("R In process: %d bytes\n", rsize);
        for (int j = 0; j < st / 4; ++j) {
            if (expected_value != buf[j])
                csize++;
        }
    }

    close(fd);
    return rsize;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(2, "Usage: test_util <file_size> <seed>\n");
        exit(1);
    }

    int file_size = atoi(argv[1]);
    uint seed = atoi(argv[2]);
    rand_seed = seed;
    if (file_size <= 0) {
        fprintf(2, "Incorrect size\n");
        exit(1);
    }
    char filename[] = "random_file";

    int wsize = create_random_file(filename, file_size);
    int rsize = read_and_verify_file(filename, file_size);
    if (rsize != wsize) {
        fprintf(2, "Error in difference between read size and write size\n");
        exit(0);
    }
    printf("Verification successful\n");
    exit(0);
}