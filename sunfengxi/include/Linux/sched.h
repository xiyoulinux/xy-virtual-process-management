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
  
	volatile long state;	/* -1 unrunnable, 0 runnable, >0 stopped */
	struct thread_info *thread_info;
	atomic_t usage;
	unsigned long flags;	/* per process flags, defined below */
	unsigned long ptrace;

	int lock_depth;		/* BKL lock depth */
	
	int prio, static_prio;
	struct list_head run_list;
	
	unsigned short ioprio;

	unsigned long sleep_avg;
	unsigned long long timestamp, last_ran;
	unsigned long long sched_time; /* sched_clock time spent running */
	int activated;

	unsigned long policy;
	
	unsigned int time_slice, first_time_slice;


	struct list_head tasks;
	/*
	 * ptrace_list/ptrace_children forms the list of my children
	 * that were stolen by a ptracer.
	 */
	struct list_head ptrace_children;
	struct list_head ptrace_list;

	struct mm_struct *mm, *active_mm;

/* task state */
	
	long exit_state;
	
	unsigned long personality;
	
	pid_t pid;
	pid_t tgid;
	/* 
	 * pointers to (original) parent process, youngest child, younger sibling,
	 * older sibling, respectively.  (p->father can be replaced with 
	 * p->parent->pid)
	 */
	struct task_struct *real_parent; /* real parent process (when being debugged) */
	struct task_struct *parent;	/* parent process */
	/*
	 * children/sibling forms the list of my children plus the
	 * tasks I'm ptracing.
	 */
	struct list_head children;	/* list of my children */
	struct list_head sibling;	/* linkage in my parent's children list */
	struct task_struct *group_leader;	/* threadgroup leader */

	/* PID/PID hash table linkage. */
	struct pid pids[PIDTYPE_MAX];



	//struct list_head cpu_timers[3];

};



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



#define remove_parent(p)	list_del_init(&(p)->sibling)
#define add_parent(p, parent)	list_add_tail(&(p)->sibling,&(parent)->children)

/*
 * 从进程表中移出进程
 * 判断进程是否为线程组的首领，是则移出进程，然后将进程从其所在的父子关系链表中移出
 */
#define REMOVE_LINKS(p) do {					\
	if (thread_group_leader(p))				\
		list_del_init(&(p)->tasks);			\
	remove_parent(p);					\
	} while (0)

/*
 * 向进程链表中添加进程
 * 判断进程是否为线程组的首领，是则添加，然后更新进程的父子关系链表
 */
#define SET_LINKS(p) do {					\
	if (thread_group_leader(p))				\
		list_add_tail(&(p)->tasks,&init_task.tasks);	\
	add_parent(p, (p)->parent);				\
	} while (0)

#define next_task(p)	list_entry((p)->tasks.next, struct task_struct, tasks)
#define prev_task(p)	list_entry((p)->tasks.prev, struct task_struct, tasks)

/*
 * 遍历系统中的每一个进程
 * init_task 系统初始化过程中“手工”创建的第一个进程的进程描述符
 */
#define for_each_process(p) \
	for (p = &init_task ; (p = next_task(p)) != &init_task ; )







#endif
