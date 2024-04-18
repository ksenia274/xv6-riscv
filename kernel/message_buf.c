#include <stdarg.h>

#include "types.h"
#include "param.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"
#include "queue.h"

queue_t queue;
char *digits = "0123456789";

void
message_buf_init(void) {
    initlock(&queue.lock, "message_buf_lock");
    queue.head = 0;
    queue.tail = 0;
}

void pr_msg_num(int xx, int base, int sign) {
    char buf[16];
    int i;
    uint x;

    if(sign && (sign = xx < 0))
        x = -xx;
    else
        x = xx;

    i = 0;
    do {
        buf[i++] = digits[x % base];
    } while ((x /= base) != 0);

    if (sign)
        buf[i++] = '-';

    while (--i >= 0)
        queue_add_char(&queue, buf[i]);
}

void pr_msg_ptr(uint64 ptr){
    int i;
    queue_add_char(&queue, '0');
    queue_add_char(&queue, 'x');
    for (i = 0; i < (sizeof(uint64) * 2); i++, ptr <<= 4)
        queue_add_char(&queue, digits[ptr >> (sizeof(uint64) * 8 - 4)]);
}

void pr_msg(const char* fmt, ...){
    if (fmt == 0){
        panic("fmt error: empty");
    }
    char* s;
    va_list ap;
    int i;
    char c;
    acquire(&tickslock);
    int cur_ticks = ticks;
    release(&tickslock);

    acquire(&queue.lock);
    queue_add_char(&queue, '[');
    pr_msg_num(cur_ticks, 10, 1);
    queue_add_char(&queue, ']');
    queue_add_char(&queue, ' ');

    va_start(ap, fmt);
    for (i = 0; (c = fmt[i]) != 0; i++) {
        if (c != '%') {
            queue_add_char(&queue, c);
            continue;
        }

        c = fmt[++i];
        if (c == 0)
            break;

        switch(c) {
            case 'd':
                pr_msg_num(va_arg(ap, int), 10, 1);
                break;
            case 'x':
                pr_msg_num(va_arg(ap, int), 16, 1);
                break;
            case 'p':
                pr_msg_ptr(va_arg(ap, uint64));
                break;
            case 's':
                if ((s = va_arg(ap, char*)) == 0)
                    s = "(null)";
                for (; *s; s++)
                    queue_add_char(&queue, *s);
                break;
            case '%':
                queue_add_char(&queue, '%');
                break;
            default:
                queue_add_char(&queue, '%');
                queue_add_char(&queue, c);
                break;
        }
    }
    va_end(ap);

    queue_add_char(&queue, '\n');
    release(&queue.lock);
}

int
sys_dmesg(void) {
    char last = 0;
    uint64 ptr, n;
    argaddr(0, &ptr);
    argaddr(1, &n);

    acquire(&queue.lock);

    int i, head = queue.head;
    for (i = 0; i < n - 1 && head != queue.tail; i++, head = (head+1) % (PAGESIZE*PAGENUM)) {
        if (copyout(myproc()->pagetable, ptr + i, &queue.data[head], 1) < 0) {
            release(&queue.lock);
            return -2;
        }
    }

    if (copyout(myproc()->pagetable, ptr + i, &last, 1) < 0) {
        release(&queue.lock);
        return -3;
    }

    release(&queue.lock);
    return 0;
}