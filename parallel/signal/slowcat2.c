#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <sys/time.h>

static int volatile loop = 0;
static void alrm_handle(int s)
{
//	alarm(1);
	loop = 1;
}

#define CPS    20
#define BUFSIZE CPS

int main(int argc, char **argv)
{
	int sfd = 0, dfd = 1;
	char buf[BUFSIZE];
	int len = 0, ret = 0, pos = 0;

	struct itimerval itv;
	
	if(argc < 2)
	{
		fprintf(stderr, "Usage: ./slowcat FILENAME\n");
		exit(1);
	}

	do
	{
		sfd = open(argv[1], O_RDONLY);
		if(sfd < 0)
		{
			if(errno != EINTR)
			{
				perror("open()");
				exit(1);
			}
		}
	}while(sfd < 0);
	
	loop = 1;
	signal(SIGALRM, alrm_handle);
//	alarm(1);
	
	itv.it_interval.tv_sec = 1;
	itv.it_interval.tv_usec = 0;
	
	itv.it_value.tv_sec = 1;
	itv.it_value.tv_usec = 0;
	if(setitimer(ITIMER_REAL, &itv, NULL) != 0)
		perror("setitime()");

	while(1)
	{
		while(!loop)
			pause;
		loop = 0;

		while((len = read(sfd,buf,BUFSIZE)) < 0)
		{
			if(errno == EINTR)
				continue;
			perror("read()");
			break;
		}
		
		if(len < 0)
			break;

		if(len == 0)
			break;
		
		pos = 0;
		while(len > 0)
		{
			ret = write(dfd, buf+pos, len);
			if(ret < 0)
			{
				if(errno == EINTR)
					continue;
				perror("write()");
				break;
			}

			len -= ret;
			pos += ret;
		}
	}

	close(sfd);
	
	exit(0);
}
	
