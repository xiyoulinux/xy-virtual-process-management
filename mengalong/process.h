/*这里的两个链表是进程队列*/
#include "cfg.h"
struct task_struct * OS_process_rdyqueue[PROCESS_MAX_NUM];
struct task_struct * OS_process_suspendqueue[PROCESS_MAX_NUM];

