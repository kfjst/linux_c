#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define PRCNUM 20
#define FILENAME "/tmp/out"
#define BUFSIZE 1024

static void func_add(void)
{
	FILE *fp;
	int fd;
	char linebuf[BUFSIZE];

	fp = fopen(FILENAME, "r+");
	if(fp == NULL)
	{
		perror("fopen()");
		exit(1);
	}
	
	fd = fileno(fp);

	lockf(fd, F_LOCK, 0);
	fgets(linebuf, BUFSIZE, fp);
	fseek(fp, 0, SEEK_SET);
	fprintf(fp,"%d\n", (atoi(linebuf) + 1));
	fflush(fp);
	lockf(fd, F_ULOCK, 0);
	
	fclose(fp);
}

int main()
{
	int i,err;
	int pid;

	for(i = 0; i < PRCNUM; i++)
	{
		pid = fork();
		if(pid < 0)
		{
			perror("fork()");
			exit(1);
		}

		if(pid == 0)
		{
			func_add();
			exit(0);
		}
	}

	for(i = 0; i < PRCNUM; i++)
	{
		wait(NULL);
	}
	
	exit(0);
}
