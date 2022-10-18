#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

static void *fun(void *p)
{
	while(1);
		pause();
	
	pthread_exit(NULL);
}

int main()
{
	int err,i;
	pthread_t tid;
	pthread_attr_t attr;

	pthread_attr_init(&attr);
	pthread_attr_setstacksize(&attr,1024*1024*1024); 
	
	for(i = 0; ;i++)
	{
		err = pthread_create(&tid,NULL, fun, NULL);
		if(err > 0)
		{
			fprintf(stderr, "pthread_create(): %s\n", strerror(err));
			break;
		}
	}
	
	printf("create thread num %d\n", i);
	pthread_attr_destroy(&attr);
	exit(0);
}
