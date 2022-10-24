#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define BUFSIZE 1024

int main()
{
	int pd[2];
	pid_t pid;
	char buf[BUFSIZE];
	ssize_t len;

	if(pipe(pd) < 0)
	{
		perror("pipe()");
		exit(1);
	}

	pid = fork();
	if(pid < 0)
	{
		perror("fork");
		exit(1);
	}

	if(pid == 0)
	{
		// child read
		close(pd[1]); //close write pd;
		len = read(pd[0], buf, BUFSIZE); 
		write(1, buf, len);
		close(pd[0]);
		exit(0);
	}
	else
	{

		//parent write
		close(pd[0]); //close read fd;
		write(pd[1], "hello", 5);
		close(pd[1]);
		wait(NULL);
		exit(0);
	}

	exit(0);
}
