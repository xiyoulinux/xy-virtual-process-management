#ifndef _SCHED_H
#define _SCHED_H

#define NR_TASKS 64
#define HZ 100

#define FIRST_TASK task[0]
#define LAST_TASK task[NR_TASKS-1]



#include <signal.h>


#define TASK_RUNNING		0
#define TASK_INTERRUPTIBLE	1
#define TASK_UNINTERRUPTIBLE	2
#define TASK_ZOMBIE		3
#define TASK_STOPPED		4

#ifndef NULL
#define NULL ((void *) 0)
#endif


struct task_struct {
  long state ;//任务的运行状态
  long counter;//任务运行的时间计数
  long priority;//运行优先数
  long signal;//信号
  struct sigaction sigaction[32];//信号执行属性结构
  long pid;//进程号
  long pgrp;//进程组号
  long father;//父进程号
  unsigned short uid;//用户id
  unsigned short euid;// 有效用户id
  unsigned short suid;//保存的用户id
  unsigned short gid;//组id
  unsigned short egid;//有效组id
  unsigned short sgid;//保存的组id
  long alarm;//报警定时值
  long utime;//用户态运行时间
  long cutime;//子进程用户态运行时间
  long start_time;//进程开始运行时刻
}


struct todo_tasks{ 
    	char *task_name; 
  	 unsigned int name_len;
  	 int sub_tasks;
  	 int subtasks_completed;
   	 struct list_head completed_subtasks;
   	 /* 已完成的子任务形成链表 */ 
  	 int subtasks_waiting; 
  	struct list_head waiting_subtasks; 
  	/* 待完成的子任务形成链表 */
 		struct list_head todo_list;
 	/* 要完成的任务形成链表 */     
}; 


extern struct task_struct *task[NR_TASKS];

extern struct task_struct *current;
extern long volatile jiffies;
extern long startup_time;



struct task_struct *task,*p
struct list_head *pos
task=&init_task;//系统中第一进程结构体
list_for_each(pos,&task->tasks) {
p=list_entry(pos, struct task_struct, tasks);
printk("%d--->%s\n",p->pid,p->comm);
   

#define CURRENT_TIME (startup_time+jiffies/HZ)



