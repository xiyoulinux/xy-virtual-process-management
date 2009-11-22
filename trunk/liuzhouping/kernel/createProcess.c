#include<stdio.h>
#include<stdlib.h>

#define MAX_PID  65535 //最多创建的进程的个数
#define RDY_STATE 0 //就绪态
#define RIG_STATE 1 //运行态
#define BLK_STATE 2 //阻塞态

int PID = 1; //全局变量

typedef struct task_struct {
	int pid;
	long state;
	struct task_struct *parent;//指向父亲的一个指针
	struct task_struct *childHead0;//指向儿子的一个链表头
	struct task_struct *child;//指向父进程的下一个儿子节点;
	struct task_struct *brother;//指向兄弟的链表（用于将父进程创建的儿子链接在一块的链表）
} task_struct;

void addToTask(task_struct *task_head,task_struct *task){
	/*新生成的节点插入到父进程的子节点队列中*/
	task->parent = task_head;
	task->childHead0 = NULL;
	task->state = RDY_STATE;
	
	if(task_head->childHead0 == NULL) { //说明task是父进程的第一个儿子
		task->brother = NULL;
		task->child = NULL;
		task_head->childHead0 = task;
	}
	else {
		task->child = task_head->childHead0;
		task_head->childHead0 = task;
		task->brother = task_head->childHead0->brother;
		task_head->childHead0 = task;
	}
}

task_struct *create(task_struct *task_head){
	task_struct *task;
	task = (task_struct *)malloc(sizeof(task_struct));
	task->pid = PID++;
	task->state = RDY_STATE;
	addToTask(task_head,task);//将当前进程用头插法链接到父亲进程的儿子链表上
	return task;
}

void init_task_struct(task_struct *task_head) {
//初始化系统的init进程
	task_head->pid = 0;
	task_head->state = RIG_STATE;
	task_head->parent = task_head;
	task_head->childHead0 = NULL;
	task_head->child = NULL;
	task_head->brother = NULL;
}

void visit_father_and_son(task_struct *task)
{
	task_struct *son,*grandson;
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	printf("process%d's father is process%d\n",task->pid,task->parent->pid);
	son = task->childHead0;
	printf("son:\n");
	while(son) {
		printf("    process%d\n",son->pid);
		//son = son->child;
		grandson = son->childHead0;
		if(grandson) 
			printf("             Grandson:\n");
		while(grandson) {
			printf("                      process%d\n",grandson->pid);
			grandson = grandson->child;
		}
		son = son->child;
	}
}
int main()
{
	task_struct *task_head;//类似于init进程
	task_head = (task_struct *)malloc(sizeof(task_struct));
	init_task_struct(task_head);//初始化init进程
	task_struct *process1;
	task_struct *process2;
	task_struct *process3;//进程123由task_head创建
	task_struct *process4;
	task_struct *process5;//进程45由进程1创建
	task_struct *process6;
	process1 = create(task_head);
	process2 = create(process1);
	process3 = create(process1);
	
	process4 = create(process1);
	process5 = create(process4);
	
	process6 = create(process4);
	visit_father_and_son(process1);
	visit_father_and_son(process4);
	visit_father_and_son(task_head);
	//visit_father_and_son(process3);
	return 0;
}
