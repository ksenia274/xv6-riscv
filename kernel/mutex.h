#pragma once
#include "sleeplock.h"


struct mutex {
    int proc_number;
    struct sleeplock lock;

};