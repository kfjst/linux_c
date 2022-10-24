#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <string.h>

#define BUFSIZE 1024

int main()
{
	pid_t pid;
	int shmid;
	char *ptr;
	
	shmid = shmget(IPC_PRIVATE, BUFSIZE, 0600);
	if(shmid < 0)
	{
		perror("shmget()");
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
		ptr = shmat(shmid, NULL, 0);
		if(ptr == (void *) -1)
		{
			perror("shmat()");
			exit(1);
		}
		strcpy(ptr, "hello");
		shmdt(ptr);
		exit(0);
	}
	else
	{
		wait(NULL);

		ptr = shmat(shmid, NULL, 0);
		if(ptr == (void *) -1)
		{
			perror("shmat()");
			exit(1);
		}
		puts(ptr);
		shmdt(ptr);
		shmctl(shmid, IPC_RMID, NULL);
		exit(0);
	}

	exit(0);
}


