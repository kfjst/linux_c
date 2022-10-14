#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>

#define CPS    20
#define BUFSIZE CPS
#define BURST 100

static volatile sig_atomic_t token = 0; //sig_atomic_t 原子类型，这种类型的遍历，取值赋值一定是原子操作
static void alrm_handle(int s)
{
	alarm(1);
	token++;
	if(token > BURST)
	{
		token = BURST;
	}
}

int main(int argc, char **argv)
{
	int sfd = 0, dfd = 1;
	char buf[BUFSIZE];
	int len = 0, ret = 0, pos = 0;

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
	
	signal(SIGALRM, alrm_handle);
	alarm(1);

	while(1)
	{
		while(token <= 0)
			pause;
		token--;

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
	
