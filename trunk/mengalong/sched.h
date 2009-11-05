#include "types.h"

struct task_struct {
	INT2U	PCBpid;				/*进程号*/
	INT2U	PCBprio;			/*进程优先级 0表示最高*/
	INT2U	PCBStat;			/*进程状态*/
};
