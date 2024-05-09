#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "sysinfo.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
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
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
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

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
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

  if(argint(0, &pid) < 0)
    return -1;
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
sys_trace(void){
  int mask;
  
  //读取trapframe 获取mask参数
  if(argint(0,&mask) < 0){ 
    return -1;
  }
  myproc()->trace_mask = mask;
  return 0;
}

//为什么不能传参呢，因为这涉及到用户态和内核态的切换，那么从用户态过来的参数，
//我们应该从a0寄存器取出
//也就是 argraw(int n) 当n=0时会取出a0寄存器的值
uint64
sys_sysinfo(void){
  //读入用户态参数
  uint64 addr;

  if(argaddr(0,&addr) < 0){
    return -1;
  }

  struct sysinfo info;
  info.freemem = count_free_mem();
  info.nproc = count_nproc();

  //使用 copyout，通过当前进程的页表，获得进程传进来的指针（逻辑地址）对应的物理地址
  //然后将 &info 中的数据复制到该指针所指位置
  if(copyout(myproc()->pagetable, addr, (char *)&info, sizeof(info)) < 0){
    return -1;
  }

  return 0;
}