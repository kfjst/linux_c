#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#define THRNUM 4

static int num = 0;

static pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

static int next(int n)
{
	n++;
	if(n >= THRNUM)
	{
		n = 0;
	}

	return n;
}
static void *fun(void *p)
{
	int n = (int)p;
	char c = 'a';
	c+=n;


	while(1)
	{
		pthread_mutex_lock(&mut);
		while(n != num)
		{
			pthread_cond_wait(&cond, &mut);
		}
		write(1, &c, 1);
		num = next(num);
		pthread_cond_broadcast(&cond);
		pthread_mutex_unlock(&mut);
	}
	
	pthread_exit(NULL);
}

int main()
{
	int i,err;
	pthread_t tid[THRNUM];
	
	for(i = 0; i < THRNUM; i++)
	{
		err = pthread_create(tid+i, NULL, fun, (void *)i);
		if(err > 0)
		{
			fprintf(stderr, "pthread_create():%s\n", strerror(err));
			exit(1);
		}
	}
	
	alarm(5); //定时销毁程序

	for(i = 0; i < THRNUM; i++)
	{
		pthread_join(tid[i], NULL);
	}

	pthread_mutex_destroy(&mut);
	pthread_cond_destroy(&cond);

	exit(1);
}
