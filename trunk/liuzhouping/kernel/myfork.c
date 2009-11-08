#include "kernel/sched.h"
#include <stdlib.h>
#include <stdio.h>
struct task_struct* copy_process();
//struct task_struct* task_init();
int put_into_run_queue(struct task_struct *p);
int schedul();
int myfork() 
{
	struct task_struct* reg_process;

	reg_process = copy_process();
	put_into_run_queue(reg_process);//将新创建的进程压入进程队列中
	schedul();
	return 0;
	//return reg_process->pid;//返回新创建进程的PID
}

struct task_struct* copy_process()
{
	int cur_time = 10;//当前时间可通过系统调用来获得当前时间
	struct task_struct* new_task;
	new_task = (struct task_struct *)malloc(sizeof(struct task_struct));
	new_task->state = cur_process->state;
	new_task->start_time = cur_time;
	new_task->count_run = 0;
	new_task->priority = PRIORITY;
	new_task->pid = PID++;
	new_task->parent = cur_process;
	//new_task->data_struct = cur_process->data_struct;//数据段复制

	return new_task;
}

int schedul()
{}
int put_into_queue(struct task_stuct *p)
{}
