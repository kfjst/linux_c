#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include "mytbf.h"

#define CPS    20
#define BUFSIZE 1024
#define BURST 100

int main(int argc, char **argv)
{
	int sfd = 0, dfd = 1;
	char buf[BUFSIZE];
	int len = 0, ret = 0, pos = 0;
	mytbf_t *tbf;
	int size;
	
	if(argc < 2)
	{
		fprintf(stderr, "Usage: ./slowcat FILENAME\n");
		exit(1);
	}
	
	tbf = mytbf_init(CPS, BURST);
	if(tbf == NULL)
	{
		fprintf(stderr, "mytbf_init() faild!\n");
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
	
	while(1)
	{
		size = mytbf_fetchtoken(tbf, BUFSIZE);
		if(size < 0)
		{
			fprintf(stderr, "mytbf_fetchtocken(): %s\n",  strerror(-size));
			break;
		}

		while((len = read(sfd,buf,size)) < 0)
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
		if(size - len > 0)
		{
			mytbf_returntoken(tbf, size-len);
		}

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
	
	mytbf_destroy(tbf);
	close(sfd);
	
	exit(0);
}
	
