#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define LEFT   30000000
#define RIGHT  30000200

struct thr_arg_st
{
	int n;
};

static void *primer(void *p)
{
	int i,j, mark;
	
	i = ((struct thr_arg_st *)p)->n;

	mark = 1;
	for(j = 2; j < i / 2; j++)
	{
		if(i % j == 0)
		{
			mark = 0;
			break;
		}
	}

	if(mark)
	{
		printf("%d is a primer\n", i);
	}
	pthread_exit(p);
}

int main()
{
	int i, err;
	pthread_t tid[RIGHT - LEFT];
	struct thr_arg_st *p;
	void *ptr;

	for(i = LEFT; i < RIGHT; i++)
	{
		p = malloc(sizeof(struct thr_arg_st));
		if(p == NULL)
		{
			fprintf(stderr, "malloc err\n");
			exit(1);
		}
		
		p->n = i;
		err = pthread_create(tid+i-LEFT, NULL, primer, (void *)p);
		if(err > 0)
		{
			fprintf(stderr, "pthread_create(): %s\n",strerror(err));
			exit(1);
		}
	}

	for(i = LEFT; i < RIGHT; i++)
	{
		pthread_join(tid[i-LEFT], &ptr);
		free(ptr);
	}

	exit(0);
}
