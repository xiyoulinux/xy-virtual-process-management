#include <stdio.h>
#include <stdlib.h>
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

#define list_entry(ptr, type, member) \
	container_of(ptr, type, member)

#define list_for_each_entry(pos, head, member)				\
	for (pos = list_entry((head)->next, typeof(*pos), member);	\
	     prefetch(pos->member.next), &pos->member != (head); 	\
	     pos = list_entry(pos->member.next, typeof(*pos), member))

#define container_of(ptr,type,member) ({ \
	const typeof( ((type *)0)->member ) *__mptr = (ptr); \
	(type *)( (char *)__mptr - offsetof(type,member) );})

/*void container_of(ptr,type,member) 
{
	const typeof((type *)0)->member *__mptr = ptr;
	(type *)( (char *)__mptr - offsetof(type,member) );
}*/

struct list_head {
	struct list_head *next, *prev;
};

static inline void INIT_LIST_HEAD(struct list_head *list)
{
	list->next = list;
	list->prev = list;
}

static inline void __list_add(struct list_head *new,
			      struct list_head *prev,
			      struct list_head *next)
{
	next->prev = new;
	new->next = next;
	new->prev = prev;
	prev->next = new;
}

#define list_for_each(pos, head) \
for (pos = (head)->next; pos != (head); \
pos = pos->next)

struct mydata {
	int data;
	struct list_head list;
};

int main()
{
	struct mydata *mylist;
	struct mydata *pos;
	struct list_head *pos1;	
	int i;
	int tmp;
	mylist = (struct mydata *)malloc(sizeof(struct mydata));
	INIT_LIST_HEAD(&(mylist->list));
	printf("请输入10个数字：");	
	for(i = 0; i < 10; i++) {
		scanf("%d",&tmp);
		pos = (struct mydata *)malloc(sizeof(struct mydata));
		pos->data = tmp;
		__list_add(&(pos->list),(mylist->list).prev,&(mylist->list).next);
	}
	list_for_each(pos1,&(mylist->list)) {
		pos= list_entry(pos1, struct mydata,list);
		printf("data: %d\n", pos->data);
	}
}
