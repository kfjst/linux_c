#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/wait.h>

#define MEMSIZE 1024

int main()
{
	char *ptr;
	int pid;

	//匿名映射，fd = -1;
	ptr = mmap(NULL,MEMSIZE, PROT_READ| PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if(ptr == MAP_FAILED)
	{
		perror("mmap()");
		exit(1);
	}

	pid = fork();
	if(pid < 0)
	{
		perror("fork()");
		exit(1);
	}
	
	if(pid == 0)
	{
		/* child write */
		strcpy(ptr, "hello shm");
		munmap(ptr, MEMSIZE);  //子进程也有映射，所以要unmap
		exit(0);
	}
	else
	{
		/* parent read */
		wait(NULL);
		puts(ptr);
		munmap(ptr, MEMSIZE);
		exit(0);
	}

	exit(0);
}
