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
        close(p[1]); //закрываем запись
        char input[100];
        int status;
        while ( (status = read(p[0], &input, 100) > 0)) {
            if (status == -1) {
                printf("read error\n");
            }
            printf("%s", input);
        }
        close(p[0]);
        exit(0);
    } else { // Родительский процесс
        printf("Parent PID: %d\n", getpid());
        printf("Child PID: %d\n", pid);
        close(p[0]); //закрываем чтение
        int len;
        for (int i = 1; i < argc; i++) {
            len = strlen(argv[i]);
            if (len >= 100) {
                close(p[1]);
                wait(0);
                printf("Overflow.\n");
            }
            if (write(p[1], argv[i], len) != len) {
                printf("write error");
                exit(1);
            }
            if (write(p[1], "\n", sizeof("\n")) != sizeof("\n")){
                printf("write error");
                exit(1);
            }
        }
        close(p[1]);
        wait(0);
        exit(0);
    }
}