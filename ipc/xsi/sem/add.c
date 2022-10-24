#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>

#define PRCNUM 20
#define FILENAME "/tmp/out"
#define BUFSIZE 1024

static int semid;

static void P(void)
{
	struct sembuf sop;
	sop.sem_num = 0;
	sop.sem_op = -1;
	sop.sem_flg = 0;
	while(semop(semid, &sop, 1) < 0)
	{
		if(errno != EAGAIN || errno != EINTR)
		{
			perror("semop");
			exit(1);
		}
	}
}

static void V(void)
{
	struct sembuf sop;
	sop.sem_num = 0;
	sop.sem_op =  1;
	sop.sem_flg = 0;
	if(semop(semid, &sop, 1) < 0)
	{
		perror("semop");
		exit(1);
	}
}

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
	
	P();
	fgets(linebuf, BUFSIZE, fp);
	fseek(fp, 0, SEEK_SET);
	fprintf(fp,"%d\n", (atoi(linebuf) + 1));
	fflush(fp);
	V();

	fclose(fp);
}

int main()
{
	int i,err;
	int pid;
	
	semid = semget(IPC_PRIVATE, 1, 0600);
	if(semid < 0)
	{
		perror("semget()");
		exit(1);
	}

	if(semctl(semid, 0, SETVAL, 1) < 0)
	{
		perror("semctl()");
		exit(1);
	}

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

	semctl(semid, 0, IPC_RMID, NULL);
	exit(0);
}
