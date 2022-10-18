#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

static void *fun(void *p)
{
	printf("pthread work\n");
	
	pthread_exit(NULL);
//	return NULL;
}

int main()
{
	int err;
	pthread_t tid;

	puts("Begin");

	err = pthread_create(&tid,NULL, fun, NULL);
	if(err > 0)
	{
		fprintf(stderr, "pthread_create(): %s\n", strerror(err));
		exit(1);
	}
	
	pthread_join(tid, NULL);
	puts("end");

	exit(0);
}
