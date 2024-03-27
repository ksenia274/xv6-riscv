
struct mutex {
    int proc_number;
    struct sleeplock *lock;

};