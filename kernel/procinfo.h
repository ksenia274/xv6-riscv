#ifndef procinfo_h
#define procinfo_h

enum procstate { UNUSED, USED, SLEEPING, RUNNABLE, RUNNING, ZOMBIE };
struct procinfo
{
    int pid;
    char name[16];
    enum procstate state;
};

#endif