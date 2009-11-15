struct list_head {
	struct list_head *next, *prev;
};

/*
下面两个宏用来找到链表头节点
*/
#define LIST_HEAD_INIT(name) { &(name), &(name) }

#define LIST_HEAD(name) \
	struct list_head name = LIST_HEAD_INIT(name)

/*
inline函数不像正常函数在调用时存在压栈和call的操作，它会把程序代码直接嵌入到调用代码段中，
也就是说使用inline函数会增大二进制程序的体积，但是会使执行速度加快。
同时，它在编译期间可以对参数进行强类型检查，这是它优于宏的一个方面。 
*/
static inline void INIT_LIST_HEAD(struct list_head *list)
{
	list->next = list;
	list->prev = list;
}

#ifndef CONFIG_DEBUG_LIST
static inline void __list_add(struct list_head *new,
			      struct list_head *prev,
			      struct list_head *next)
{
	next->prev = new;
	new->next = next;
	new->prev = prev;
	prev->next = new;
}
#else

/*
extern可以置于变量或者函数前，以标示变量或者函数的定义在别的文件中，提示编译器遇到此变量和函数时在其他模块中寻找其定义。
另外，extern也可用来进行链接指定。
*/
extern void __list_add(struct list_head *new,
			      struct list_head *prev,
			      struct list_head *next);
#endif

#ifndef CONFIG_DEBUG_LIST
static inline void list_add(struct list_head *new, struct list_head *head)
{
	__list_add(new, head, head->next);
}
#else
extern void list_add(struct list_head *new, struct list_head *head);
#endif

static inline void list_add_tail(struct list_head *new, struct list_head *head)
{
	__list_add(new, head->prev, head);
}

/*
smp_wmb()防止编译器和CPU优化代码执行的顺序。在这里，smp_wmb保证在它之前的两行代码执行完了之后再执行后两行.如果没有smp_wmb，代码在CPU中执行的顺序可能和源代码里的不一样! 
*/

/*
RCU（Read-Copy Update）通过延迟写操作来提高同步性能.

RCU常用来保护读操作占多数的链表与数组。具有RCU的链表的操作函数与普通链表操作函数的区别是在函数名后加上了_rcu，如list_for_each_rcu函数。

函数list_for_each_rcu的功能是遍历一个rcu保护的链表。其中，参数pos表示用来做链表位置计数的&struct list_head结构，参数head表示链表头。只要遍历被rcu_read_lock()保护，使用诸如list_add_rcu()的函数对链表同时访问是安全的。
*/
static inline void __list_add_rcu(struct list_head * new,
		struct list_head * prev, struct list_head * next)
{
	new->next = next;
	new->prev = prev;
	smp_wmb();
	next->prev = new;
	prev->next = new;
}

static inline void list_add_rcu(struct list_head *new, struct list_head *head)
{
	__list_add_rcu(new, head, head->next);
}

static inline void list_add_tail_rcu(struct list_head *new,
					struct list_head *head)
{
	__list_add_rcu(new, head->prev, head);
}

static inline void __list_del(struct list_head * prev, struct list_head * next)
{
	next->prev = prev;
	prev->next = next;
}
