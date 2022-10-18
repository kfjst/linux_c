#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

static void cleanup_fun(void *p)
{
	printf("%s\n", (char *)p);
}

static void *fun(void *p)
{
	pthread_cleanup_push(cleanup_fun, "cleanup 1");
	pthread_cleanup_push(cleanup_fun, "cleanup_2");
	pthread_cleanup_push(cleanup_fun, "cleanup_3");

	puts("push over");
	
//	pthread_cleanup_pop(1);
//	pthread_cleanup_pop(1);
//	pthread_cleanup_pop(1);

	pthread_exit(NULL);
	

//pop 必须要有，可以放在调用不到的地方，调用不到的地方，入参默认为真
	pthread_cleanup_pop(1);
	pthread_cleanup_pop(0);
	pthread_cleanup_pop(0);
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
