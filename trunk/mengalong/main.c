#include "includes.h"

int main()
{
	char ch = 'y';
	unsigned short  pid;
//	PCB pcb;
	struct task_struct pcb;

	while(ch != 'q') {
		printf(">");	
		ch = getchar();
		if('n' == ch) {
			getchar();
			
			pcb = create_process();
			printf(">create process sucess! %d\n", pcb.PCBpid);
			
		} else {
		}
	}
	return 0;
}
