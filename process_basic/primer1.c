#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"

#define LEFT  30000
#define RIGHT 30200
int main()
{
	int i = 0,j,mark;
	pid_t pid;

	for(i = LEFT; i <= RIGHT; i++)
	{
		pid = fork();
		if(pid < 0)
		{
			perror("fork()");
			exit(1);
		}
		
		if(pid == 0)
		{

			mark = 1;
			for(j = 2; j < i /2; j++)
			{
				if(i%j == 0)
				{
			
					mark = 0;
					break;
				}
			}
		
			if(mark)
			{
				printf("%d is a primer\n",i);
			}
	//		sleep(1000);
			exit(0); //处理完成后结束
		}
	}
	
	//	sleep(1000);
	exit(0);
}
