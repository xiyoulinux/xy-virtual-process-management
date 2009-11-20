#include<stdio.h>
#include<stdlib.h>

#define MAX_PID  65535 //最多创建的进程的个数
#define RDY_STATE 0 //就绪态
#define RIG_STATE 1 //运行态
#define BLK_STATE 2 //阻塞态

int PID = 1; //全局变量

typedef struct task_struct{
	long pid,father;
	long state;
	struct task_struct *parent;//指向父亲的一个指针
	struct task_struct *childHead0;//指向儿子的一个链表头
	struct task_struct *brother;//指向兄弟的链表（用于将父进程创建的儿子链接在一块的链表）
}task_struct;

void addToTask(task_struct *task_head,task_struct *task){
//将一个进程所创建的子进程链接在一起
	task_struct *childHead;//儿子链表的头结点（虚拟的，只是一个引导作用）
	childHead = (task_struct *)malloc(sizeof(task_struct));
	childHead->pid = 65535;
	childHead->father = 65535;
	childHead->parent = NULL;
	childHead->childHead0 = NULL;
	childHead->brother = NULL;
	task_head->childHead0 = childHead;
	task->brother = childHead->brother;
	childHead->brother = task;
	task->parent = task_head;
	free(childHead);
}

task_struct *create(task_struct *task_head,int father){
	task_struct *task;
	task = (task_struct *)malloc(sizeof(task_struct));
	task->pid = PID++;
	task->father = father;
	task->state = RDY_STATE;
	addToTask(task_head,task);//将当前进程用头插法链接到父亲进程的儿子链表上
	printf("~~~~~~~~~~~~~~~~~~~~~~~~\n");
	printf(" task->pid= %ld\n task->father= %ld\n task->state= %ld\n",task->pid,task->father,task->state);//打印当前进程的信息
	return task;
}

void init_task_struct(task_struct *task_head){
//初始化系统的init进程
	task_head->pid = 0;
	task_head->father = 0;
	task_head->state = RIG_STATE;
	task_head->parent = task_head;
	task_head->childHead0 = task_head;
	task_head->brother = task_head;
}

int main(){
	task_struct *task_head;//类似于init进程
	task_head = (task_struct *)malloc(sizeof(task_struct));
	init_task_struct(task_head);//初始化init进程
	task_struct *process1;
	task_struct *process2;
	task_struct *process3;//进程123由task_head创建
	task_struct *process4;
	task_struct *process5;//进程45由进程1创建
	process1 = create(task_head,task_head->pid);
	process2 = create(task_head,task_head->pid);
	process3 = create(task_head,task_head->pid);
	
	process4 = create(process1,process1->pid);
	process5 = create(process1,process1->pid);
	
	
	return 0;
}
