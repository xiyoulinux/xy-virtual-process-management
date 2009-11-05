#include "includes.h"
//#include "types.h"

struct task_struct create_process()
{
	struct task_struct stk;

	stk.PCBpid = 0;
	stk.PCBprio = 4;
//	stk.PCBStat = STAT_RDY;
	
	return stk;
}
