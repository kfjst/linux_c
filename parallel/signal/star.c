#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>


static void signalhandler(int i)
{
	write(1, "!", 1);
}

int main()
{
	int i = 0;
	
	/* 忽略 中止信号(SIGINT), SIG_IGN:忽略信号 */
//	signal(SIGINT, SIG_IGN);
 	signal(SIGINT, signalhandler);
	
	for(i = 0 ; i < 10; i++)
	{
		write(1,"*",1);
		sleep(1);
	}
	

	exit(0);
}
