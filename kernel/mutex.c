#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "mutex.h"
#include "proc.h"


struct mutex_table {
    struct spinlock table_splock; // Спин-лок для защиты таблицы
    struct mutex mutex_list[NMUTEX];  // Глобальная таблица мьютексов
};

struct mutex_table mutex_table;

void init_mutex(void) {
    initlock(&mutex_table.table_splock, "spinlock");
    for (int i = 0 ; i < NMUTEX; ++i) {
        initsleeplock(&mutex_table.mutex_list[i].lock, "sleeplock");
        mutex_table.mutex_list[i].proc_number = 0;
    }
}
int create_mutex(void ) {
    acquire(&mutex_table.table_splock);
    for (int i = 0; i < NMUTEX; i++) {
        if (mutex_table.mutex_list[i].proc_number == 0) {
            mutex_table.mutex_list[i].proc_number += 1;
            release(&mutex_table.table_splock);
            return i;
        }
    }
    release(&mutex_table.table_splock);

    return -1;
}

int lock_mutex(int mutex_desc) {
    if (mutex_desc < 0 || mutex_desc >= NMUTEX) {
        return -1; // Ошибка при блокировке мьютекса
    }
    acquiresleep(&mutex_table.mutex_list[mutex_desc].lock);
    return 0; // Успех
}

int unlock_mutex(int mutex_desc) {
    if (mutex_desc < 0 || mutex_desc >= NMUTEX) {
        return -1; // Ошибка при блокировке мьютекса
    }
    if (!holdingsleep(&mutex_table.mutex_list[mutex_desc].lock)) {
        return -2;
    }
    releasesleep(&mutex_table.mutex_list[mutex_desc].lock);
    return 0; // Успех
}

int destroy_mutex(int mutex_desc) {
    if (mutex_desc < 0 || mutex_desc >= NMUTEX) {
        return -1; // Ошибка при блокировке мьютекса
    }
    acquire(&mutex_table.table_splock);
    if (holdingsleep(&mutex_table.mutex_list[mutex_desc].lock) < 0) {
        release(&mutex_table.table_splock);
        return -2;
    }
    if (unlock_mutex(mutex_desc) < 0){
        release(&mutex_table.table_splock);
        return -3;
    }
    if (mutex_table.mutex_list[mutex_desc].proc_number == 0) {
        release(&mutex_table.table_splock);
        return -4;
    }

    mutex_table.mutex_list[mutex_desc].proc_number -= 1;
    release(&mutex_table.table_splock);
    return 0;

}

int sys_createmutex(void) {
    return create_mutex();
}

int sys_lockmutex(void) {
    int mutex_desc;
    argint(0, &mutex_desc);
    return lock_mutex(mutex_desc);
}

int sys_unlockmutex(void) {
    int mutex_desc;
    argint(0, &mutex_desc);
    return unlock_mutex(mutex_desc);
}

int sys_destroymutex(void) {
    int mutex_desc;
    argint(0, &mutex_desc);
    return destroy_mutex(mutex_desc);
}