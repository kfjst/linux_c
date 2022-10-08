#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/wait.h"

int main()
{
	pid_t pid = 0;

	puts("Begin!");
	fflush(NULL);

	pid = fork();
	if(pid < 0)
	{
		perror("fork()");
		exit(1);
	}

	if(pid == 0)
	{
		execl("/bin/sleep", "httpd", "100", NULL);
		perror("execl()");
		exit(1);
	}

	wait(NULL);
	
	puts("end !");
	exit(0);
}
