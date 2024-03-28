#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"
#include "kernel/mutex.h"

int main(int argc, char** argv){
    if (argc < 2) {
        fprintf(2, "Not enough arguments\n");
        exit(1);
    }

    int ch[2];
    int par[2];
    if (pipe(ch) < 0 || pipe(par) < 0){
        fprintf(2,"pipe error");
        exit(3);
    }
    int mutex = createmutex();
    if (mutex < 0){
        fprintf(2, "mutex error");
    }

    int pid = fork();
    if (pid < 0){
        fprintf(2,"fork error");
        exit(4);
    } else if (pid == 0){ //child process
        close(ch[1]);
        close(par[0]);
        char buf;
        int res;
        while ((res = read(ch[0], &buf, 1)) > 0) {
            lockmutex(mutex);
            printf("%d: received %c\n", getpid(), buf);
            unlockmutex(mutex);
            write(par[1], &buf, 1);
        }
        close(ch[0]);
        close(par[1]);
        if (res < 0){
            fprintf(2,"read error");
            destroymutex(mutex);
            exit(2);
        }
        destroymutex(mutex);
        exit(0);
    } else {          //parent process
        close(ch[0]);
        close(par[1]);
        write(ch[1], argv[1], strlen(argv[1]));
        close(ch[1]);
        char buf;
        int res;
        while ((res = read(par[0], &buf, 1)) > 0) {
            lockmutex(mutex);
            printf("%d: received %c\n", getpid(), buf);
            unlockmutex(mutex);
        }
        close(par[0]);
        if (res < 0) {
            printf("read error\n");
            destroymutex(mutex);
            exit(2);
        }
        int status;
        wait(&status);
    }
    destroymutex(mutex);
    exit(0);
}