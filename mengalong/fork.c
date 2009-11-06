
#include "includes.h"
/*创建进程的函数*/
struct task_struct * create_process()
{
	struct task_struct *stk;
	stk = (struct task_struct *)malloc(sizeof( struct task_struct));

	stk->PCBpid = 0;
	stk->PCBprio = 4;
	stk->PCBstat = STAT_RDY;
	
	return  stk;
}
