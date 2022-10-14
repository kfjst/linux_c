#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main()
{
	time_t end = 0;
	int64_t count = 0;

	end = time(NULL) + 5;

	while(time(NULL) <= end)
	{
		count++;
	}

	printf("%ld\n", count);

	exit(0);
}





