#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char **argv) {
    int p[2];
    pipe(p);
    if (argc < 2){
        printf("wrong number of arguments\n");
        exit(1);
    }
    if (pipe(p) == -1){
        printf("pipe error\n");
        exit(1);
    }
    int pid;
    pid = fork();

    if (pid < 0) {
        printf("fork error\n");
        exit(1);
    } else if (pid == 0) { // Дочерний процесс
        close(p[1]);
        close(0);
        dup(p[0]);
        close(p[0]);
        char *argv[] = {"/wc", 0};
        exec("/wc", argv);
        printf("error in wc exec\n");
        exit(-1);
    } else { // Родительский процесс
        printf("Parent PID: %d\n", getpid());
        printf("Child PID: %d\n", pid);
        close(p[0]); //закрываем чтение
        char input[100];
        int len;
        for (int i = 1; i < argc; i++) {
            len = strlen(argv[i]);
            if (len >= 100) {
                close(p[1]);
                wait(0);
                printf("Overflow.\n");
            }
            strcpy(input, argv[i]);
            write(p[1], input, len);
            write(p[1], "\n", sizeof("\n"));
        }
        close(p[1]);
        wait(0);
        exit(0);
    }
}