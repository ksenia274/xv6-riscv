#include "types.h"
#include "param.h"
#include "riscv.h"
#include "spinlock.h"
#include "defs.h"
#include "queue.h"

void queue_add_char(queue_t *q, char c) {
    acquire(&(q->lock));
    q->data[q->tail] = c;
    q->tail = (q->tail + 1) % (PAGESIZE * PAGENUM);
    if (q->tail == q->head) {
        q->head = (q->head + 1) % (PAGESIZE * PAGENUM);
    }
    release(&(q->lock));
}