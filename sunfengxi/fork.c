#include<stdio.h>
#include<stdlib.h>
#include <sched.h>

long last_pid=0;

int copy_process(int nr)
{
	struct task_struct *p;
	int i;
	struct file *f;

	p = (struct task_struct *) malloc(sizeof(struct task_struct));
	if (!p){
	      printf("\tOut of memory !");			
		return -1;
	}
	task[nr] = p;
	*p = *current;	
	p->state = TASK_UNINTERRUPTIBLE;
	p->pid = last_pid;
	p->father = current->pid;
	p->counter = p->priority;
	p->signal = 0;
	p->alarm = 0;
	p->leader = 0;		
	p->utime = p->stime = 0;
	p->cutime = p->cstime = 0;
	p->start_time = jiffies;	
	p->state = TASK_RUNNING;	/* do this last, just in case */
	return last_pid;
}

int find_empty_process(void)
{
	int i;

	repeat:
		if ((++last_pid)<0) last_pid=1;
		for(i=0 ; i<NR_TASKS ; i++)
			if (task[i] && task[i]->pid == last_pid) goto repeat;
	for(i=1 ; i<NR_TASKS ; i++)
		if (!task[i])
			return i;
	return -1;
}
