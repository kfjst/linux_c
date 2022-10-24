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
		close(pd[1]);
		dup2(pd[0], 0);
		close(pd[0]);
		fd = open("/dev/nll", O_RDWR);
		execl("/usr/bin/mpg123", "mpg123", "-", NULL);
		perror("execl()");
		eixt(1);
	}
	else
	{

		//parent write
		close(pd[0]); //close read fd;
		// 父进程从网上收取数据，往管道中写,这部分是伪码
		
		//write(pd[1]), xxx,xxx );
		close(pd[1]);
		wait(NULL);
		exit(0);
	}

	exit(0);
}
