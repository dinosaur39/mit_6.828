#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "sysinfo.h"

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

uint64
sys_trace(void)
{
  int trace;
  argint(0, &trace);
  myproc()->trace = trace;
  printf("trace syscall: %d\n", trace);
  return 0;
}

uint64
sys_sysinfo(void)
{
  struct sysinfo *info_p;
  struct proc *p = myproc();
  uint64 freemem_size, nproc, addr; 
  int rc;

  nproc = nproc_count();
  freemem_size = get_freemem_size();
  p = myproc();
  argint(0, (int*)&info_p);
  addr = (uint64)info_p;

  rc = copyout(p->pagetable, addr, (char*)&freemem_size, sizeof(uint64));
  if (rc)
    return -1;
  rc = copyout(p->pagetable, addr + sizeof(uint64), (char*)&nproc, sizeof(uint64));
  if (rc)
    return -1;
  return 0;
}
