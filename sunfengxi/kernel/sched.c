#define rt_task(p) (unlikely((p)->prio<MAX_RT_PRIO))
#define MAX_USER_RT_PRIO 100
#define MAX_RT_PRIO   MAX_USER_RT_PRIO
#define MAX_PRIO (MAX_RT_PRIO+40)  
#define NICE_TO_PRIO(nice)	(MAX_RT_PRIO + (nice) + 20)
#define PRIO_TO_NICE(prio)	((prio) - MAX_RT_PRIO - 20)

#define unlikely(x)	__builtin_expect(!!(x), 0)

#define CURRENT_BONUS(p) \
	(NS_TO_JIFFIES((p)->sleep_avg) * MAX_BONUS / \
		MAX_SLEEP_AVG)



static int effective_prio(task_t *p)
{
	int bonus, prio;

	if (rt_task(p))
		return p->prio;

	bonus = CURRENT_BONUS(p) - MAX_BONUS / 2;

	prio = p->static_prio - bonus;
	if (prio < MAX_RT_PRIO)
		prio = MAX_RT_PRIO;
	if (prio > MAX_PRIO-1)
		prio = MAX_PRIO-1;
	return prio;
}

#define BITMAP_SIZE ((((MAX_PRIO+1+7)/8)+sizeof(long)-1)/sizeof(long))


/*
*当系统创建新进程并投入运行时，首先将该进程插入到对应可执行队列的队尾
*然后根据进程的优先级设置优先级位图中的相位
*
*
*/
typedef struct runqueue runqueue_t;

/*优先级数组*/
struct prio_array {
	unsigned int nr_active;/*优先级数组中可运行进程的数目*/
	unsigned long bitmap[BITMAP_SIZE];/*优先级位图*/
	struct list_head queue[MAX_PRIO];/*保存可执行进程队列*/
};

/*
 *运行队列的数据结构
 *
 */
struct runqueue {
	spinlock_t lock;/*自旋锁，保护其所在的运行队列数据结构的一致性*/

	unsigned long nr_running;/*记录运行队列包含进程的数目*/

	unsigned long long nr_switches;/*记录当前处理机上发生进程切换的次数，每次进程切换时，加1*/
	
	unsigned long nr_uninterruptible;/*记录该运行队列中产生不可中断睡眠进程的次数*/

	unsigned long expired_timestamp;/*记录上一次发生超时，活动优先级数组交换时的时间戳*/
	
	unsigned long long timestamp_last_tick;/*记录系统当前的时间*/
	
	task_t *curr；/*指向运行队列中当前正在运行的进程*/
	task_t *idle;/*指向运行队列中空闲的进程*/
	
	prio_array_t *active;/*指向活动优先级数组*/
	prio_array_t *expired;/*指向超时优先级数组*/
	prio_array_t arrays[2];/*实际存储了以上两个数组*/
	
	int best_expired_prio;/*记录超时优先级数组中所有进程中的最高静态优先级*/
	atomic_t nr_iowait;//记录因访问io而阻塞的进程

};
/*唤醒等待队列中的进程*/
static void __wake_up_common(wait_queue_head_t *q, unsigned int mode,int nr_exclusive, int sync, void *key)
{
	struct list_head *tmp, *next;

	list_for_each_safe(tmp, next, &q->task_list) {
		wait_queue_t *curr;
		unsigned flags;
		curr = list_entry(tmp, wait_queue_t, task_list);
		flags = curr->flags;
		if (curr->func(curr, mode, sync, key) && (flags & WQ_FLAG_EXCLUSIVE) && !--nr_exclusive)
			break;
	}
}














