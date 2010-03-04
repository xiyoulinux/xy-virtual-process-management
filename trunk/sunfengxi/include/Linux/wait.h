#ifndef _LINUX_WAIT_H
#define _LINUX_WAIT_H

/*等待队列的节点（一个代表一个阻塞状态的进程）*/
typedef struct __wait_queue wait_queue_t;

struct __wait_queue {
	unsigned int flags;
	void *private;/*睡眠进程的进程描述符地址*/
	wait_queue_func_t func;/*函数指针，唤醒进程的方法*/
	struct list_head task_list;
};

/*等待队列*/
struct __wait_queue_head {
	spinlock_t lock;/*自旋锁，保护其所在的运行队列数据结构的一致性*/
	struct list_head task_list;/*睡眠在该等待队列上的所有进程构成的链表的表头*/
};


typedef struct __wait_queue_head wait_queue_head_t;

/*初始化等待队列*/
static inline void init_waitqueue_head(wait_queue_head_t *q)
{
	spin_lock_init(&q->lock);/*初始化自旋锁变量为SPIN_LOCk_UNLOCKED*/
	INIT_LIST_HEAD(&q->task_list);
}


/*初始化等待队列节点*/
static inline void init_waitqueue_entry(wait_queue_t *q, struct task_struct *p)
{
	q->flags = 0;
	q->private = p;
	q->func = default_wake_function;/*系统默认的唤醒函数*/
}













#endif
