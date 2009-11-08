#include "kernel/myfork.c"

int main()
{
	int pid;
	pid = myfork();
	return 0;
}
