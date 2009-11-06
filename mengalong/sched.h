/*本文件下面用来定义相关的解构体*/
#include "types.h"

struct task_struct {
	INT2U	PCBpid;				/*进程号*/
	INT2U	PCBprio;			/*进程优先级 0表示最高*/
	INT2U	PCBstat;			/*进程状态*/
};
