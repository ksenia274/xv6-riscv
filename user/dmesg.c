#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"


int main (int argc, char *argv[]){
    char* m = malloc(PAGENUM * PAGESIZE + 1);

    if (dmesg(m) < 0) {
        fprintf(2, "Adress error: unknown\n");
        exit(1);
    }

    printf("%s", m);
    exit(0);
}