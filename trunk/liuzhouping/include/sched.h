#include "include/data.h"

/*
 以下5行是进程的5种状态
 */
#define TASK_RUNNING 1
#define TASK_INTERRUPTIBLE 10
#define TASK_UNINTERRRUPTIBLE 100
#define TASK_ZOMBIE 1000
#define TASK_STOPPED 10000
#define PRIORITY 10

typedef int pid_t;
struct task_struct {
	int state;//进程状态
	/*进程退出状态*/
	//int exit_state;
	//int exit_code;
	//int exit_signal;
	struct data_struct *data;
	/*进程开始时间*/
	long start_time;
	/*进程结束时间*/
	long end_time;
	/*进程被调度的次数*/
	int count_run;
	/*进程运行总共用的时间*/
	long count_time;
	/*进程优先级*/
	int priority;
	/*进程ID*/
	pid_t pid;//父进程号
	pid_t tgid;//祖先进程号
	
	/*进程亲缘关系*/
	struct task_struct *real_parent;
	struct task_struct *parent;
//	struct task_struct *group_leader;
	
	/*进程的内存布局信息*/
//	struct mm_struct *mm;
	/*打开的文件描述符*/
	//struct files_struct *files;
}

struct task_struct *process_list;//进程队列
struct task_struct *block_list;//阻塞队列
struct task_struct *run_list;//运行队列

struct task_struct *cur_process;//当前进程
cur_process = (task_struct *)malloc(sizeof(struct task_struct));
