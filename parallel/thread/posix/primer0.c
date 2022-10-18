#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define LEFT   30000000
#define RIGHT  30000200

static void *primer(void *p)
{
	int i,j, mark;
	
	i = (int)p;

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
	pthread_exit(NULL);
}

int main()
{
	int i, err;
	pthread_t tid[RIGHT - LEFT];

	for(i = LEFT; i < RIGHT; i++)
	{
		err = pthread_create(tid+i-LEFT, NULL, primer, (void *)i);
		if(err > 0)
		{
			fprintf(stderr, "pthread_create(): %s\n",strerror(err));
			exit(1);
		}
	}

	for(i = LEFT; i < RIGHT; i++)
	{
		pthread_join(tid[i-LEFT], NULL);
	}

	exit(0);
}
