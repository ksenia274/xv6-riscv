#pragma once

typedef struct queue {
    struct spinlock lock;
    char data[PAGESIZE * PAGENUM];
    int head, tail;
} queue_t;

void queue_add_char(queue_t *q, char c);