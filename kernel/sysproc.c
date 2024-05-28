#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

int accessed(pagetable_t pg, uint64 pointer, int page){
    int ans = 0;
    for (int i = 0; i < page; ++i) {
        pte_t *pte = walk(pg, pointer, 0);
        if (*pte & PTE_A) {
            ans = 1;
            *pte &= ~PTE_A;
        }
        pointer += (1L<<12);
    }
    return ans;
}

uint64 sys_pgaccess(void) {
    pagetable_t pg = myproc()->pagetable;
    uint64 res;
    int page;
    uint64 pointer;
    argaddr(0, &pointer);
    argint(1, &page);
    argaddr(2, &res);
    page = (page + (1L<<12) - 1)/ (1L<<12);
    int ans = accessed(pg, pointer, page);
    if (copyout(pg, res, (char *)&ans, sizeof(ans)) < 0) {
        return -1;
    }

    return 0;
}
