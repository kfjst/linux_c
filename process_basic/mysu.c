#include "stdio.h"
#include "stdlib.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/*
	make mysu

	
root:chown root mysu
root:chmod u+s mysu
user:./mysu 0 cat /etc/shadow

*/

int main(int argc, char **argv)
{
	pid_t pid = 0;
	
//	printf("%d\n", argc);
	if(argc < 4)
	{
		fprintf(stderr, "Usage ...\n");
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
		setuid(atoi(argv[1]));
		execvp(argv[2], argv+2);
		perror("execvp()");
		exit(1);
	}

	wait(NULL);


	exit(0);
}

