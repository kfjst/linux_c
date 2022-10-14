#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "anytime.h"

static void fun(void *argv)
{
	printf( "%s\n", (char *)argv);
}

int main()
{
	int id = 0;
	char arg1[] = "job1";
	char arg2[] = "job2";
	char arg3[] = "job3";

	printf("Begin\n");
	
	id = at_addjob(10, fun, arg1);
	if(id < 0)
		fprintf(stderr, "at_addjob(): %s\n", strerror(-id));

	id = at_addjob(1, fun, arg2);
	if(id < 0)
		fprintf(stderr, "at_addjob(): %s\n", strerror(-id));

	id = at_addjob(5, fun, arg3);
	if(id < 0)
		fprintf(stderr, "at_addjob(): %s\n", strerror(-id));

	printf("end\n");

	while(1);
	
	exit(0);
}


