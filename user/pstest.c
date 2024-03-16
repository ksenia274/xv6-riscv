#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void test_correct(){
    int bufsize = 32;
    struct procinfo plist[bufsize];
    while (1) {
        if (plist == 0) {
            printf("test_correct failed!!!\n");
            return;
        }
        int lim = ps_listinfo(plist, bufsize);
        if (lim == -1) {
            bufsize *= 2;
            continue;
        } else {
            break;
        }
    }
    printf("test_correct passed\n");
}

void test_wrong_buf(){
    int bufsize = 32;
    struct procinfo plist[bufsize];
    if (plist == 0) {
        printf("test_wrong_buf failed!!!\n");
        return;
    }
    int lim = ps_listinfo(plist, 1);
    if (lim == -1) {
        printf("test_wrong_buf passed\n");
    } else {
        printf("test_wrong_buf failed!!!\n");
    }
}

int main(){
    test_correct();
    test_wrong_buf();
    exit(0);
}