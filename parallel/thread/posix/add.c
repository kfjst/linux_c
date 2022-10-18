#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#define THRNUM 20
#define FILENAME "/tmp/out"
#define BUFSIZE 1024

static pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
static void *add(void *p)
{
	FILE *fd;
	char linebuf[BUFSIZE];

	fd = fopen(FILENAME, "r+");
	if(fd == NULL)
	{
		perror("fopen()");
		exit(1);
	}
	
	pthread_mutex_lock(&mut);
	fgets(linebuf, BUFSIZE, fd);
	fseek(fd, 0, SEEK_SET);
//	sleep(1); //让并发更严重, 问题更容易复现
	fprintf(fd,"%d\n", (atoi(linebuf) + 1));
	fclose(fd);
	pthread_mutex_unlock(&mut);
	
	pthread_exit(NULL);
}

int main()
{
	int i,err;
	pthread_t tid[THRNUM];
	
	for(i = 0; i < THRNUM; i++)
	{
		err = pthread_create(tid+i, NULL, add, NULL);
		if(err > 0)
		{
			fprintf(stderr, "pthread_create():%s\n", strerror(err));
			exit(1);
		}
	}

	for(i = 0; i < THRNUM; i++)
	{
		pthread_join(tid[i], NULL);
	}
	
	pthread_mutex_destroy(&mut);
	exit(0);
}
