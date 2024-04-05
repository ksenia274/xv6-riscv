#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

static char *states[] = {
[UNUSED]    "unused",
[USED]      "used",
[SLEEPING]  "sleep",
[RUNNABLE]  "runnable",
[RUNNING]   "running",
[ZOMBIE]    "zombie"
};

int main (int argc, char** argv){
    int bufsize = 8;
    int lim;

    struct procinfo plist[bufsize];
    while (1){

        lim = ps_listinfo(plist, bufsize);
        if (lim == -1) {
            bufsize *= 2;
            continue;
        } else {
            break;
        }
    }
    struct procinfo* curp;
    for (int i = 1; i <= lim; ++i) {
        curp = plist + i;
        printf("Process - name: %s, state: %s, parent PID: %d\n",
               curp->name,
               states[curp->state],
               curp->pid);
    }
    exit(0);

}