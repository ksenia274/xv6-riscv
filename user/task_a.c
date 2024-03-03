#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main() {
    int pid;
    pid = fork();

    if (pid < 0) {
        printf("fork error\n");
        exit(1);
    } else if (pid == 0) { // Дочерний процесс
        int sleep_time = 100; // Генерируем случайное время ожидания от 5 до 15 секунд
        sleep(sleep_time);
        exit(1);
    } else { // Родительский процесс
        printf("Parent PID: %d\n", getpid());
        printf("Child PID: %d\n", pid);
        int status;
        wait(&status);
        printf("Child process %d exited with status %d\n", pid, status);

    }

    return 0;
}
