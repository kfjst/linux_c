#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#define MYRTSIG  (SIGRTMIN+6)

static void signalhandler(int i)
{
	write(1, "!", 1);
}

int main()
{
	int i,j;
	sigset_t set, oset, saveset;
	
	/* 忽略 中止信号(SIGINT), SIG_IGN:忽略信号 */
//	signal(SIGINT, SIG_IGN);
 	signal(MYRTSIG, signalhandler);
	sigemptyset(&set);
	sigaddset(&set, MYRTSIG);
	sigprocmask(SIG_UNBLOCK, &set, &saveset); //保留进入模块前的状态，在退出时恢复
	sigprocmask(SIG_BLOCK, &set, &oset);
	for(j = 0; j < 1000; j++)
	{
	//	sigprocmask(SIG_BLOCK, &set, &oset);
		for(i = 0 ; i < 5; i++)
		{
			write(1,"*",1);
			sleep(1);
		}

		write(1,"\n",1);
		sigsuspend(&oset); //相当于下面三条语句的原子操作
/*	
		sigset_t tmpset;
		sigprocmask(SIG_SETMASK, &oset, &tmpset);
		pause();
		sigprocmask(SIG_SETMASK, &tmpset, NULL);
*/
	}
	sigprocmask(SIG_SETMASK, &saveset,NULL);

	exit(0);
}
