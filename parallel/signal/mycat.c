#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define BUFSIZE 20

int main(int argc, char **argv)
{
	int sfd = 0, dfd = 1;
	char buf[BUFSIZE];
	int len = 0, ret = 0, pos = 0;

	if(argc < 2)
	{
		fprintf(stderr, "Usage: ./mycat FILENAME\n");
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
		len = read(sfd,buf,BUFSIZE);
		if(len < 0)
		{
			if(errno == EINTR)
				continue;
			perror("read()");
			break;
		}

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
	
