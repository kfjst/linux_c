#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

static int volatile loop = 1;

static void alrm_handler(int s)
{
	loop = 0;
}

int main()
{
	int64_t count = 0;

	signal(SIGALRM, alrm_handler);
	alarm(5);

	while(loop)
	{
		count++;
	}

	printf("%ld\n", count);

	exit(0);
}





