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
	int i,j;
	sigset_t set, saveset;
	
	/* 忽略 中止信号(SIGINT), SIG_IGN:忽略信号 */
//	signal(SIGINT, SIG_IGN);
 	signal(SIGINT, signalhandler);
	sigemptyset(&set);
	sigaddset(&set, SIGINT);
	sigprocmask(SIG_UNBLOCK, &set, &saveset);
	for(j = 0; j < 1000; j++)
	{
		sigprocmask(SIG_BLOCK, &set,NULL);
		for(i = 0 ; i < 5; i++)
		{
			write(1,"*",1);
			sleep(1);
		}

		write(1,"\n",1);
		sigprocmask(SIG_UNBLOCK, &set,NULL);
	}
	sigprocmask(SIG_SETMASK, &saveset,NULL);

	exit(0);
}
